# 🎮 Tetris Game - C++ Console Project

![Banner](https://img.shields.io/badge/Project-Tetris-blue?style=for-the-badge&logo=c%2B%2B)
![Course](https://img.shields.io/badge/Course-SS004.Q12-green?style=for-the-badge)
![UIT](https://img.shields.io/badge/School-UIT-orange?style=for-the-badge)

> Đồ án môn học **Kỹ năng nghề nghiệp (SS004.Q12)** - Trường Đại học Công nghệ Thông tin (UIT), ĐHQG-HCM.

## 📖 Giới thiệu (Introduction)

Dự án này là phiên bản tái hiện lại trò chơi **Tetris** kinh điển, được xây dựng bằng ngôn ngữ **C++**. Trò chơi được phát triển bởi nhóm **Freeze** với mục tiêu áp dụng các kiến thức lập trình hướng đối tượng, quản lý bộ nhớ và kỹ năng làm việc nhóm.

Trò chơi bao gồm đầy đủ các cơ chế cốt lõi của Tetris hiện đại như: **Super Rotation System (SRS)**, **Bag-7 Randomizer**, và hệ thống âm thanh sống động.

## ✨ Tính năng nổi bật (Features)

* 👾 **Gameplay cổ điển:** Xếp gạch, xóa hàng và ghi điểm.
* 🎒 **Cơ chế Bag-7:** Đảm bảo 7 loại khối (I, J, L, O, S, T, Z) xuất hiện đều đặn, tránh việc quá lâu không có khối I.
* 🔄 **Xoay khối thông minh:** Xử lý va chạm khi xoay gần tường hoặc gần các khối khác (Wall Kick cơ bản).
* 🎵 **Âm thanh & Nhạc nền:** Tích hợp BGM và hiệu ứng âm thanh (SFX) khi di chuyển, xoay, ăn điểm.
* 👀 **Next Piece:** Xem trước khối tiếp theo để tính toán chiến thuật.
* 🚀 **Hard Drop & Soft Drop:** Thả chậm hoặc thả ngay lập tức để tiết kiệm thời gian.
* 📊 **Hệ thống tính điểm:** Tăng điểm dựa trên số dòng xóa được cùng lúc (Single, Double, Triple, Tetris).



## 🎮 Hướng dẫn điều khiển (Controls)

| Phím | Chức năng |
| :---: | :--- |
| **⬅️ / ➡️** | Di chuyển khối sang Trái / Phải |
| **⬆️** | Xoay khối 90 độ |
| **⬇️** | Thả chậm (Soft Drop) - Tăng tốc rơi |
| **Space** | Thả ngay lập tức (Hard Drop) |
| **Esc** | Tạm dừng / Thoát game |

## 🛠️ Cài đặt & Chạy chương trình (Installation)

### Yêu cầu hệ thống
* Hệ điều hành: Windows
* IDE: Visual Studio 2019/2022 (khuyến nghị) hoặc Dev-C++, CLion.
* Ngôn ngữ: C++11 trở lên.

### Cách build từ Source Code

1.  **Clone repository này về máy:**
    ```bash
    git clone [https://github.com/24520332/DoAn_SS004.Q12.git](https://github.com/24520332/DoAn_SS004.Q12.git)
    ```
2.  **Mở dự án:**
    * Khởi động Visual Studio.
    * Chọn `Open a project or solution`.
    * Tìm đến file `.sln` trong thư mục vừa clone.
3.  **Cài đặt tài nguyên:**
    * Đảm bảo các file âm thanh và hình ảnh nằm đúng thư mục `Resources` hoặc cùng cấp với file `.exe` sau khi build.
4.  **Build & Run:**
    * Nhấn `F5` hoặc nút **Local Windows Debugger** để chạy game.

## 👥 Thành viên nhóm Freeze (Team Members)

| STT | Họ và tên | MSSV | Vai trò chính |
|:---:|:---|:---:|:---|
| 1 | **Võ Minh Đức** | 24520332 | 👑 Leader, Core Logic, Game Loop |
| 2 | **Trần Minh Đức** | 24520330 | 💻 UI Design, Report (LaTeX) |
| 3 | **Nguyễn Thị Mỹ Duyên** | 24520408 | 📋 Project Manager (Trello), Collision Logic |
| 4 | **Bùi Công Định** | 24520303 | ⚙️ Gameplay Logic, Bug Fixing |
| 5 | **Vũ Minh Quân** | 24521460 | 🧮 Algorithm, Audio System |

## 📚 Tài liệu tham khảo (References)

* [Tetris Wiki - Tetris Guideline](https://tetris.wiki/Guideline)
* [Hard Drop Wiki - Super Rotation System](https://harddrop.com/wiki/SRS)
* [C++ Reference](https://en.cppreference.com/)

## 🤝 Lời cảm ơn (Acknowledgments)

Xin gửi lời cảm ơn chân thành đến **Th.S Nguyễn Văn Toàn** đã hướng dẫn và hỗ trợ nhóm trong suốt quá trình thực hiện đồ án môn học Kỹ năng nghề nghiệp.

---
Made with ❤️ by **Team Freeze**.
