#!/usr/bin/env python3
import csv
import os
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent
os.chdir(ROOT)

LOAD = "0.8x"
OBJECTIVES = ("LLB", "DCB")
GENETIC_VALUES = ("0", "1")
BANDS = "2"
TIME_LIMIT = "14400"
PARTITION = "court"
JOB_PREFIX = "RETMB_jobsbatch_"
EXPECTED_BASE_CASES = 81
EXPECTED_EXPERIMENTS = 324
BATCH_SIZE = 100

PARAMETER_VALUES = {
    "GNPY_activation": "0",
    "QoTFolder": "none",
    "ChromaticDispersion_activation": "1",
    "OSNR_activation": "1",
    "Bands": BANDS,
    "All_paths_compute": "0",
    "All_paths_print": "0",
    "TFlow_Policy": "2",
    "Reinforcements": "0",
    "Relaxation_variables": "0",
    "Genetic_iterations": "50",
    "Genetic_population": "500",
    "Genetic_crossing": "100",
    "Genetic_mutation": "200",
    "Genetic_chosenK": "10",
    "Genetic_extraK": "15",
    "ActivateLB": "1",
    "CPLEX_Strategy": "0",
    "nbDemandsAtOnce": "1000",
    "formulation": "2",
    "userCuts": "0",
    "allowBlocking": "0",
    "hopPenalty": "0",
    "partitionPolicy": "0",
    "partitionLoad": "0",
    "partitionSlice": "0",
    "solver": "0",
    "method": "0",
    "preprocessingLevel": "2",
    "linearRelaxation": "0",
    "relaxMethod": "0",
    "outputPath": "../Instances/Dump/",
    "outputLevel": "0",
    "nbSlicesInOutputFile": "0",
    "globalTimeLimit": TIME_LIMIT,
    "timeLimit": TIME_LIMIT,
}

RESULT_HEADER = (
    "LinkS;TranspS;Instance;Demands;UB;LB;GAP;Status;Time;NodesProcessed;"
    "NodesRemaining;TotalCplexCuts;TerminationReason;OF;Formulation;CD;OSNR;"
    "GNpy;Bands;Reinforcements;Cuts;Prepro;Genetic;It;Pop;Cross;Mut;cK;eK;"
    "GenTime;GenSol;GenSolIt;GenSolTime;LBactive;ComputedLB;CplexStrategy;"
    "PreproTime;Variables;Constraints;v0;preproC;Paths;FeasibleC;InfeasibleC;"
    "OnlyOsnrC;OnlyReachC;FeasibleL;InfeasibleL;OnlyOsnrL;OnlyReachL;ads;dcb;"
    "llb;nlus;slus;suld;trl;tus;tase;programTime"
)


def replace_parameters(template, values):
    replaced = set()
    output = []
    for line in template:
        stripped = line.strip()
        if "=" in stripped and not stripped.startswith("*"):
            key = stripped.split("=", 1)[0]
            if key in values:
                output.append(f"{key}={values[key]}\n")
                replaced.add(key)
                continue
        output.append(line)
    missing = set(values) - replaced
    if missing:
        raise RuntimeError(f"Parameters absent from template: {sorted(missing)}")
    return output


def discover_cases():
    load_root = ROOT / "Instances" / LOAD
    if not load_root.is_dir():
        raise RuntimeError(f"Missing instance directory: {load_root}")
    cases = []
    for transponder in sorted(p.name for p in load_root.iterdir() if p.is_dir()):
        transponder_root = load_root / transponder
        for topology in sorted(p.name for p in transponder_root.iterdir() if p.is_dir()):
            demands_root = transponder_root / topology / "Demands"
            for demand_dir in sorted((p for p in demands_root.iterdir() if p.is_dir()), key=lambda p: p.name):
                link_file = transponder_root / topology / "Links" / demand_dir.name / "Link.csv"
                if not link_file.is_file():
                    raise RuntimeError(f"Missing link file for {demand_dir}")
                cases.append((transponder, topology, demand_dir.name))
    if len(cases) != EXPECTED_BASE_CASES:
        raise RuntimeError(f"Expected {EXPECTED_BASE_CASES} base cases, found {len(cases)}")
    return cases


def write_job(batch_number, parameter_files, jobs_template):
    job_name = f"{JOB_PREFIX}{batch_number}.sh"
    lines = []
    for line in jobs_template:
        if line.startswith("#SBATCH --array="):
            lines.append(f"#SBATCH --array=0-{len(parameter_files)-1}\n")
        elif line.startswith("#SBATCH --partition="):
            lines.append(f"#SBATCH --partition={PARTITION}\n")
        elif line.startswith("#SBATCH --output="):
            lines.append(f"#SBATCH --output=batch_{batch_number}_%a\n")
        else:
            lines.append(line)
    stems = " ".join("/" + p.stem for p in parameter_files)
    lines.extend([
        "\n",
        f"tab1=({stems})\n",
        f"echo parametersSet/batch_{batch_number}/${{tab1[$SLURM_ARRAY_TASK_ID]}}\n",
        f"./exec parametersSet/batch_{batch_number}/${{tab1[$SLURM_ARRAY_TASK_ID]}}.txt "
        f">> executionOutputs${{tab1[$SLURM_ARRAY_TASK_ID]}}.txt\n",
    ])
    (ROOT / job_name).write_text("".join(lines))
    return job_name


def main():
    template = (ROOT / "../../Inputs/onlineParametersBase.txt").resolve().read_text().splitlines(True)
    jobs_template = (ROOT / "../../Inputs/jobsBase.sh").resolve().read_text().splitlines(True)
    cases = discover_cases()

    parameters_root = ROOT / "parametersSet"
    outputs_root = ROOT / "executionOutputs"
    qot_root = ROOT / "QoTSet"
    for path in (parameters_root, outputs_root, qot_root):
        if path.exists():
            shutil.rmtree(path)
        path.mkdir()

    rows = []
    generated = []
    counter = 0
    for transponder, topology, demand_folder in cases:
        demand_code = demand_folder.removesuffix("_demands")
        for objective in OBJECTIVES:
            for genetic in GENETIC_VALUES:
                counter += 1
                batch = (counter - 1) // BATCH_SIZE + 1
                batch_dir = parameters_root / f"batch_{batch}"
                batch_dir.mkdir(exist_ok=True)
                values = dict(PARAMETER_VALUES)
                values.update({
                    "topologyFile": f"Instances/{LOAD}/{transponder}/{topology}/Links/{demand_folder}/Link.csv",
                    "initialMappingDemandFile": "",
                    "initialMappingAssignmentFile": "",
                    "demandToBeRoutedFolder": f"Instances/{LOAD}/{transponder}/{topology}/Demands/{demand_folder}",
                    "Genetic": genetic,
                    "Genetic_metric": objective,
                    "obj": objective,
                })
                stem = (
                    f"oP_L{LOAD}_T{transponder[:3]}_i{topology}_d{demand_code}_of{objective}"
                    f"_f2_tf2_cd1_os1_gn0_b{BANDS}_r0_cu0_p2_g{genetic}"
                    "_it50_p500_c100_m200_cK10_eK15_lb1_cp0"
                )
                parameter_path = batch_dir / f"{stem}.txt"
                parameter_path.write_text("".join(replace_parameters(template, values)))
                generated.append(parameter_path)
                rows.append([LOAD, transponder, topology, demand_code, objective, "22", "1", "1", "0", BANDS,
                             "0", "0", "2", genetic, "50", "500", "100", "200", "10", "15", "1", "0"])

    if counter != EXPECTED_EXPERIMENTS:
        raise RuntimeError(f"Expected {EXPECTED_EXPERIMENTS} experiments, generated {counter}")

    with (ROOT / "experimentList.csv").open("w", newline="") as handle:
        writer = csv.writer(handle, delimiter=";", lineterminator="\n")
        writer.writerow(["LinkS", "TranspS", "Instance", "Demands", "OF", "Formulation", "CD", "OSNR",
                         "GNpy", "Bands", "Reinforcements", "Cuts", "Prepro", "Genetic", "It", "Pop",
                         "Cross", "Mut", "cK", "eK", "LBactive", "CplexStrategy"])
        writer.writerows(rows)

    batches = []
    for batch_number in range(1, (counter - 1) // BATCH_SIZE + 2):
        parameter_files = sorted((parameters_root / f"batch_{batch_number}").glob("*.txt"))
        batches.append(write_job(batch_number, parameter_files, jobs_template))
    (ROOT / "script.sh").write_text("".join(f"sbatch {name}\necho {name} >> okBatchs.txt\n" for name in batches))
    (ROOT / "results.csv").write_text(RESULT_HEADER)
    (outputs_root / "about.txt").write_text("outputs from hpc\n")
    print(f"Generated {counter} experiments in {len(batches)} batches.")


if __name__ == "__main__":
    main()
