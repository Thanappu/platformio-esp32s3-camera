import os

def display_repo_tree(startpath, output_file):
    """
    สร้างโครงสร้างแบบ tree ของ repository สำหรับโปรเจกต์ C++ / PlatformIO
    โดยไม่รวมบางไดเรกทอรีที่ไม่จำเป็น และแสดงเนื้อหาของไฟล์สำคัญ (เช่น .cpp, .hpp, .ini)
    """
    # ===== ไดเรกทอรีที่ไม่ต้องการแสดง =====
    EXCLUDED_DIRS = {
        '.env', '.git', '__pycache__', '.venv', 'env', 'node_modules',
        '.pio', '.vscode', 'build', 'cmake-build-debug', 'cmake-build-release'
    }

    # ===== นามสกุลไฟล์ที่ต้องการแสดงเนื้อหา =====
    INCLUDED_EXTENSIONS = ('.cpp', '.hpp', '.h', '.c', '.ino', '.ini', '.txt', '.md', '.json', '.yaml', '.yml')

    # ===== ไฟล์สำคัญที่ควรอยู่ใน tree =====
    IMPORTANT_FILES = {
        'CMakeLists.txt', 'platformio.ini', 'README.md', 'README.txt',
        'Makefile', '.gitignore', 'requirements.txt', 'package.json'
    }

    def tree_symbols(level, is_last):
        """สร้างสัญลักษณ์สำหรับโครงสร้าง tree"""
        if level == 0:
            return ""
        prefix = "".join("    " if l else "│   " for l in is_last[:-1])
        connector = "└── " if is_last[-1] else "├── "
        return prefix + connector

    def should_include_item(path):
        """ตรวจสอบว่าไฟล์หรือโฟลเดอร์นี้ควรถูกรวมอยู่ใน tree หรือไม่"""
        basename = os.path.basename(path)

        # ข้ามโฟลเดอร์ที่อยู่ใน EXCLUDED_DIRS
        if os.path.isdir(path) and basename in EXCLUDED_DIRS:
            return False

        if os.path.isdir(path):
            return True  # รวมโฟลเดอร์ที่เหลือทั้งหมด

        # รวมไฟล์สำคัญ หรือไฟล์ที่มีนามสกุลที่กำหนดไว้
        return basename in IMPORTANT_FILES or path.endswith(INCLUDED_EXTENSIONS)

    # ===== เขียนทับไฟล์อย่างปลอดภัย =====
    tmp_file = output_file + ".tmp"

    # ลบไฟล์ temp เก่าถ้ามี
    if os.path.exists(tmp_file):
        os.remove(tmp_file)

    # ลบไฟล์จริงเก่าด้วย (กันไฟล์ซ้อน)
    if os.path.exists(output_file):
        os.remove(output_file)

    with open(tmp_file, 'w', encoding='utf-8') as out_file:
        out_file.write(os.path.basename(startpath) + '\n')

        def walk_dir(path, level=0, is_last_list=[]):
            """Recursive: เดินอ่านโฟลเดอร์ทั้งหมด"""
            try:
                entries = sorted([
                    entry for entry in os.listdir(path)
                    if should_include_item(os.path.join(path, entry))
                ])
            except OSError as e:
                out_file.write(f"{tree_symbols(level, is_last_list + [True])}[Error accessing: {os.path.basename(path)} - {e}]\n")
                return

            for i, entry in enumerate(entries):
                full_path = os.path.join(path, entry)
                is_current_last = (i == len(entries) - 1)
                current_is_last_list = is_last_list + [is_current_last]

                out_file.write(tree_symbols(level + 1, current_is_last_list) + entry + '\n')

                if os.path.isdir(full_path):
                    walk_dir(full_path, level + 1, current_is_last_list)
                elif os.path.isfile(full_path) and entry.endswith(INCLUDED_EXTENSIONS):
                    # แสดงเนื้อหาไฟล์ (แต่ไม่อ่านไฟล์ binary หรือใหญ่เกิน)
                    try:
                        size_kb = os.path.getsize(full_path) / 1024
                        if size_kb > 100:
                            out_file.write("    " * (level + 1) + f"# [Skipped: {entry} ({size_kb:.1f} KB)]\n")
                            continue

                        content_prefix = "".join("    " if l else "│   " for l in current_is_last_list) + "    "
                        out_file.write(f"{content_prefix}# Contents of {entry}:\n")
                        with open(full_path, 'r', encoding='utf-8', errors='ignore') as f:
                            for line in f:
                                out_file.write(f"{content_prefix}{line.rstrip()}\n")
                            out_file.write('\n')
                    except Exception as e:
                        out_file.write(f"{content_prefix}[Error reading file: {e}]\n")

        walk_dir(startpath)

    # เมื่อเขียนครบ -> ย้าย temp ไปเป็นไฟล์จริง
    os.replace(tmp_file, output_file)


# --- วิธีใช้ ---
if __name__ == "__main__":
    repo_path = './'  # โฟลเดอร์โปรเจกต์ปัจจุบัน
    output_filename = 'output_tree_full.txt'

    if os.path.isdir(repo_path):
        print(f"Generating repository tree for: {os.path.abspath(repo_path)}")
        display_repo_tree(repo_path, output_filename)
        print(f"Repository tree saved to: {output_filename}")
    else:
        print(f"Error: Directory not found at {repo_path}")
