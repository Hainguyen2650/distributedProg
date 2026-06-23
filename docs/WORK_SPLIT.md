# Phan chia cong viec cho 4 thanh vien

Tai lieu nay chi dung de chia viec lap trinh va chuan bi thi nghiem. Chua phai bao cao cuoi cung.

## Thanh vien 1: Thuat toan SUMMA va MPI

- Phu trach `src/summa_kernel.c` va `include/summa_kernel.h`.
- Kiem tra process grid `q x q`.
- Quan ly `MPI_Cart_create`, row communicator va column communicator.
- Cai dat vong lap SUMMA:
  - Broadcast block A theo hang.
  - Broadcast block B theo cot.
  - Nhan tich cuc bo vao block C.
- Dam bao `N` chia het cho `sqrt(P)`.

## Thanh vien 2: Matrix va kiem chung

- Phu trach `src/matrix.c` va `include/matrix.h`.
- Tao du lieu dau vao:
  - Pattern.
  - Identity.
  - Random co seed.
- Cai dat nhan ma tran tuan tu lam baseline.
- Tinh sai so lon nhat giua ket qua song song va ket qua tuan tu.
- Ho tro in ma tran nho de debug.

## Thanh vien 3: Do thoi gian va can bang tai

- Phu trach `src/timing.c` va `include/timing.h`.
- Thu thap:
  - Thoi gian tinh toan.
  - Thoi gian truyen thong.
  - Thoi gian tong.
  - Thoi gian ranh uoc luong.
  - Byte gui va nhan uoc luong.
- Tong hop min, max, average.
- Kiem tra mat can bang tai theo nguong 25%.

## Thanh vien 4: CLI, CSV, thi nghiem va bieu do

- Phu trach `src/args.c`, `include/args.h`, `src/output.c`, `include/output.h`, `Makefile`.
- Them tuy chon dong lenh.
- Tao CSV cho cac bieu do:
  - Tong thoi gian theo `N`.
  - Thoi gian tinh toan va truyen thong theo tung process.
  - Speedup khi thay doi so process.
- Chuan bi script chay nhieu kich thuoc neu can.

## Lenh build va chay mau

```bash
cd /mnt/parallel
make
mpirun --hostfile /mnt/parallel/hosts/host -np 4 /mnt/parallel/summa --n 8 --csv --per-rank-csv
```

Neu SSH host key chua duoc cau hinh:

```bash
mpirun \
  --mca plm_rsh_args "-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null" \
  --mca btl_tcp_disable_family 6 \
  --hostfile /mnt/parallel/hosts/host \
  -np 4 /mnt/parallel/summa --n 8 --csv --per-rank-csv
```
