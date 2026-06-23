# SUMMA experiment data

Generated: 2026-06-23T16:39:23+07:00

Build command: make
MPI command template: mpirun --mca btl self,vader --oversubscribe -np <P> /mnt/parallel/summa --n <N> --csv --per-rank-csv --output-prefix <prefix> [--no-verify]

Files:
- summary.csv: all cleaned summary rows from every run.
- per_rank.csv: all cleaned per-rank timing rows from every run.
- size_sweep_np4.csv: chart data for runtime versus N at P=4.
- speedup_n1536.csv: speedup and efficiency for valid square process counts P=1,4,9 at N=1536.
- granularity_np4_n1536.csv: per-process compute/communication/idle data for load-balance discussion.
- raw/*.log: complete stdout/stderr logs for each run.

Notes:
- SUMMA implementation requires P to be a perfect square because it creates a q x q process grid. P=2 and P=8 are therefore invalid for this code.
- Correctness was verified on N=64 for P=1 and P=4; both verification_error values are 0.
- Larger timing runs used --no-verify, so their verify_error field is -1 by design.
- Runs used local shared-memory MPI transport and --oversubscribe so P=4 and P=9 could run on this two-core container.
