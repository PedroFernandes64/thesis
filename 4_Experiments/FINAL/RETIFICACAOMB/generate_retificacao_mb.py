#!/usr/bin/env python3
import runpy
from pathlib import Path

# Load the standalone generator body kept beside this wrapper.  The copied
# body is intentionally local so the RETIFICACAOMB directory remains portable.
runpy.run_path(str(Path(__file__).with_name("retificacao_mb_impl.py")), run_name="__main__")
