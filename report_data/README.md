# SUMMA experiment data

Generated: 2026-06-23

Experiment:
- Matrix size `N = 32`
- MPI processes `P = 4`
- Process grid `sqrt(P) x sqrt(P) = 2 x 2`
- Local block per process: `16 x 16`

Command:

```bash
mpirun --mca btl self,vader --oversubscribe -np 4 /mnt/parallel/summa --n 32 --csv --per-rank-csv --output-prefix n32_p4
```

Files:
- `summary.csv`: cleaned summary row for the run.
- `per_rank.csv`: cleaned per-rank timing rows.
- `raw/n32_p4.log`: complete stdout/stderr log.

Notes:
- Verification was enabled.
- `verification_error` is `0`, so the parallel result matches the serial baseline.
- This implementation requires `P` to be a perfect square and `N` to be divisible by `sqrt(P)`.
