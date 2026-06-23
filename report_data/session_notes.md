# Experiment Session Notes

Date: 2026-06-23

## Cluster Setup

- Master: `192.168.1.2` (`master`)
- Worker 1: `192.168.1.8` (`slave1`)
- Worker 2: `192.168.1.3` (`slave2`)
- MPI implementation: Open MPI `5.0.10`
- Shared project path: `/mnt/distributedProg`
- `slave2:/mnt/distributedProg` was mounted from `master:/mnt/distributedProg` through NFS.
- Passwordless SSH from `master` to all nodes was configured and verified.

The active hostfile was updated to:

```text
192.168.1.2 slots=5
192.168.1.3 slots=6
192.168.1.8 slots=5
```

## Stable Experiment Plan

The SUMMA implementation requires a square process grid, so valid process counts are:

```text
P = 1, 4, 9, 16, ...
```

Although the cluster has 16 advertised slots after the `5-6-5` setup, `P=16` was not used for final results because the multi-node Open MPI runtime hung during tiny smoke tests. The stable and reproducible setup is:

```text
P = 1, 4, 9
N = 6000
```

For `P=9`, the intended mapping is 3 MPI ranks per node:

```bash
mpirun --hostfile hosts/host --map-by ppr:3:node --bind-to core -np 9 ./summa --n 6000 --no-verify --csv --per-rank-csv --output-prefix n6000_p9
```

## Successful Results

The following runs completed successfully with `N=6000` and `--no-verify`:

| Processes | Total time (s) | Compute time (s) | Communication time (s) | Total speedup | Compute speedup |
|---:|---:|---:|---:|---:|---:|
| 1 | 113.105301 | 112.194917 | 0.734363 | 1.000000 | 1.000000 |
| 4 | 63.054117 | 45.187377 | 30.479243 | 1.793781 | 2.482882 |
| 9 | 45.457886 | 27.794446 | 19.348266 | 2.488134 | 4.036595 |

Chart-ready speedup data is stored in:

```text
report_data/speedup_n6000.csv
```

Raw logs are stored in:

```text
report_data/raw/n6000_p1.log
report_data/raw/n6000_p4.log
report_data/raw/n6000_p9.log
```

## P=16 Investigation

`P=16` was tested with `N=16`, which should finish immediately. It did not complete reliably across the three-node setup. Observations:

- `P=16` works on a single node, so the SUMMA code and square-grid requirement are not the cause.
- Multi-node `P=16` hangs during MPI communication or shutdown.
- SSH, NFS, hostfile visibility, and basic IP reachability were verified.
- Worker firewalls were inactive.
- Hostname resolution was cleaned so `master`, `slave1`, and `slave2` resolve to `192.168.1.x` addresses instead of loopback aliases.
- Open MPI still reported TCP BTL reachability warnings such as `Unable to find reachable pairing between local and remote interfaces`.

Conclusion: `P=16` was excluded because the issue is in the multi-node Open MPI runtime/transport configuration, not the SUMMA implementation.

## Report Recommendation

Use `P=1,4,9` for the speedup chart and discuss `P=16` as a cluster runtime limitation. The `P=9` run is the best stable distributed result for the available machines.
