# Integração das retificações SB e MB

## Motivo

Os resultados T-Flow P2 com `LBactive=1` foram produzidos com um limite superior indevido na restrição `MinSliceLeavingEdgeInternalDemandSminK`. A correção substitui esse teto por `exp2.getTrivialUb()`.

Não filtre nem remova linhas pela coluna numérica `LB`. Também **não remova `LBactive=0`**: essas linhas são os baselines necessários e não são regeneradas pelas retificações. Mesmo quando a intenção for apagar os resultados antigos com VI, não se deve apagar indiscriminadamente todo `LBactive=1`, pois isso removeria também o Flow. O filtro de substituição deve sempre combinar `Formulation=22 AND LBactive=1` com a carga e os objetivos indicados abaixo.

## Single-band

O novo `RETIFICACAOSB/results.csv` deve substituir, no conjunto antigo, exatamente as linhas com:

- `LinkS=1.5x`;
- `Formulation=22` (T-Flow P2);
- `LBactive=1`;
- `OF` em `3,4,8` (TUS, TRL e NLUS);
- `Genetic` em `0,1`.

São 486 linhas: `81 casos x 3 objetivos x 2 valores de Genetic`.

Preservar todas as linhas Flow, DR-AOV e T-Flow com `LBactive=0`. A pasta antiga `RETIFICACAO` não deve ser reaplicada integralmente: suas 162 linhas Flow continuam válidas, mas suas 162 linhas T-Flow são substituídas pela nova retificação.

### Pedido para a revisão futura do SB

> Integre em modo conservador `RETIFICACAOSB/results.csv` ao conjunto single-band anterior. Substitua exclusivamente as 486 configurações `LinkS=1.5x`, `Formulation=22`, `LBactive=1`, objetivos TUS/TRL/NLUS e `Genetic=0/1`, usando a chave experimental completa. Preserve todos os baselines `LBactive=0`, todas as linhas Flow e DR-AOV e todos os arquivos originais. Confirme 486 substituições sem ausências ou duplicatas, refaça as seis tabelas principais, as quatro tabelas do apêndice e a seção de resultados, e compare os números novos com os anteriormente publicados.

## Multibanda

Primeiro, retirar da análise todas as linhas com `LinkS=0.5x`, conforme decisão autoral. No subconjunto `0.8x`, o novo `RETIFICACAOMB/results.csv` deve substituir exatamente as linhas com:

- `Formulation=22` (T-Flow P2);
- `LBactive=1`;
- `OF` em `20,2020` (LLB e DCB);
- `Genetic` em `0,1`.

São 324 linhas: `81 casos x 2 objetivos x 2 valores de Genetic`. Depois da substituição, o conjunto multibanda `0.8x` deve conter 1.296 configurações únicas.

Preservar todas as linhas Flow e todas as linhas T-Flow com `LBactive=0`. Não concatenar simplesmente os CSVs: remover as 324 chaves antigas contaminadas e inserir as 324 retificadas.

### Pedido para a análise futura do MB

> Analise os resultados multibanda usando somente `LinkS=0.8x`. Integre conservadoramente `RETIFICACAOMB/results.csv` ao CSV original, substituindo exclusivamente as 324 configurações `Formulation=22`, `LBactive=1`, objetivos LLB/DCB e `Genetic=0/1`, pela chave experimental completa. Exclua da análise todos os casos `0.5x`, preserve Flow e T-Flow com `LBactive=0`, confirme 1.296 configurações únicas após a integração e confronte os resultados retificados com os outputs antes de produzir tabelas, figuras ou conclusões.
