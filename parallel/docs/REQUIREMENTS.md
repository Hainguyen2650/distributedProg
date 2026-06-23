# Yeu cau bai tap

Nguon: noi dung yeu cau bang tieng Viet do giang vien cung cap.

## Bao cao

Viet bao cao tu 10-20 trang, toi da 20 trang.

## Noi dung can trinh bay

- Song song cap do nao: tac vu hay du lieu.
- Su dung ky thuat phan ra nao: data, exploratory, recursive, speculative, hybrid?
- Cach thuc song song hoa:
  - Phan bo cho cac tien trinh/bo xu ly nhu the nao.
  - Mapping technique / processor-process assignment: 1D, 2D, `n/sqrt(p) * n/sqrt(p)` block?
  - Mo ta cach thuc giao tiep.
  - Communication strategy and topology: blocking or non-blocking, master-slave, topology tree, ring, tubecube, ...
  - Co ap dung ky thuat can bang tai khong?
  - Load balancing considerations.
  - Ma gia thuat toan song song.

## Results

- Kiem tra xem ket qua cua chuong trinh song song co chinh xac la loi giai cua bai toan neu ra khong.
- Xac dinh kich thuoc du lieu dau vao cua bai toan:
  - Chon so luong tien trinh bang so luong nhan cua CPU.
  - Vi du 3 may moi may 4 nhan thi chon tong so tien trinh la 12.
  - Ve bieu do thoi gian chay trong 2 truong hop:
    - Co thoi gian truyen thong.
    - Khong co thoi gian truyen thong.
  - Hai truc:
    - Thoi gian chay cua chuong trinh, tu luc bat dau den luc ket thuc toan bo chuong trinh.
    - Kich thuoc du lieu dau vao cua bai toan.
  - Xac dinh kich thuoc du lieu `N` sao cho thoi gian chay cua chuong trinh khoang tu 2-3 phut.
- Kiem tra tinh min granularity:
  - Kich thuoc du lieu tren moi tac vu neu song song hoa dua tren tac vu.
  - Hoac kich thuoc du lieu tren moi tien trinh neu song song hoa dua tren du lieu.
  - Chon kich thuoc du lieu dau vao cho toan bo chuong trinh la `N`.
  - Chon so luong tien trinh bang so luong nhan cua CPU.
  - Vi du 3 may moi may 4 nhan thi chon tong so tien trinh la 12.
  - Ve bieu do thoi gian chay cua tung tien trinh tinh ca thoi gian truyen thong.
  - Mot cot la mot tien trinh.
  - Thoi gian tinh toan va thoi gian truyen thong duoc ve chung cot nhung co mau khac nhau.
  - Xac dinh xem he thong co can bang tai khong.
  - Neu khong, tuc thoi gian ranh cua 2 tien trinh bat ki lech qua 25%, thi chinh lai do min: min hon hoac tho hon.
- Kiem tra do tang toc:
  - Chon kich thuoc du lieu dau vao la `2*N`.
  - Bien doi so luong tien trinh tu `1, 2, 4, 8, ..., 2X`, voi `X` la tong so tien trinh ung voi so nhan vat ly cua cac CPU.
  - Ve bieu do thoi gian chay trong 2 truong hop:
    - Co thoi gian truyen thong.
    - Khong co thoi gian truyen thong.
  - Kem theo bieu do do tang toc tuong ung.

## Ghi chu thuc hien

- Chua viet bao cao trong buoc nay.
- Code can ho tro thu thap so lieu de sau nay viet bao cao va ve bieu do.
- Code nen duoc chia thanh cac phan ro rang de 4 thanh vien co the cung lam.
