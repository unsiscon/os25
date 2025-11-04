#!/bin/bash
# ===========================================
# 多版本程序性能测试脚本
# 作者：ChatGPT（优化版）
# ===========================================

# ====== 1. 初始化阶段 ======
set -e
mkdir -p data_files
RESULT_LOG="result.log"
rm -f $RESULT_LOG

# ====== 2. 测试参数配置 ======
# 数据集大小（自动从1M到4G）
sizes=("1M" "10M" "100M" "1G" "2G" "3G" "4G")

# 线程/进程参数（可按实际代码调整）
THREADS=4
PROCS=4

# 随机数据 or 零数据
USE_RANDOM=true    # 改为 false 则使用 /dev/zero（生成更快）

# ====== 3. 编译阶段 ======
echo "========== 编译阶段 ==========" | tee -a $RESULT_LOG

compile_cpp() {
    src="$1.cpp"
    exe="$1"
    echo "正在编译 $src ..."
    g++ -std=c++17 -O2 -pthread -o "$exe" "$src"
}

compile_cpp single
compile_cpp thread
compile_cpp proc

# ====== 4. 数据文件生成阶段 ======
echo "========== 数据文件生成阶段 ==========" | tee -a $RESULT_LOG

for size in "${sizes[@]}"; do
    file="data_files/data_${size}.bin"

    if [ ! -f "$file" ]; then
        echo "生成 $file ..."
        case "$size" in
            *M)
                bs=1M
                count=${size%M}
                ;;
            *G)
                bs=1G
                count=${size%G}
                ;;
            *)
                echo "未知单位：$size"
                exit 1
                ;;
        esac

        if [ "$USE_RANDOM" = true ]; then
            dd if=/dev/urandom of="$file" bs=$bs count=$count status=progress
        else
            dd if=/dev/zero of="$file" bs=$bs count=$count status=progress
        fi
    else
        echo "$file 已存在，跳过生成。"
    fi
done

# ====== 5. 性能测试阶段 ======
echo "========== 性能测试阶段 ==========" | tee -a $RESULT_LOG

# 检查 time 命令路径
if command -v /usr/bin/time >/dev/null 2>&1; then
    TIME_CMD="/usr/bin/time -f '%E'"
elif command -v time >/dev/null 2>&1; then
    TIME_CMD="time -p"
else
    echo "未找到 time 命令，请安装（Debian/Ubuntu: apt install time）"
    exit 1
fi

run_test() {
    prog=$1
    echo -e "\n---- 测试程序：$prog ----" | tee -a $RESULT_LOG

    for size in "${sizes[@]}"; do
        file="data_files/data_${size}.bin"
        echo "[${file}] 正在测试..." | tee -a $RESULT_LOG

        case $prog in
            single)
                $TIME_CMD ./$prog "$file" "output_${prog}_${size}.txt" 2>&1 | tee -a $RESULT_LOG
                ;;
            thread)
                # total_items 根据大小估算（1字节1项，仅作演示）
                $TIME_CMD ./$prog "$file" $THREADS 1000000 2>&1 | tee -a $RESULT_LOG
                ;;
            proc)
                $TIME_CMD ./$prog "$file" $PROCS 1000000 2>&1 | tee -a $RESULT_LOG
                ;;
        esac

        echo "" >> $RESULT_LOG
    done
}

run_test single
run_test thread
run_test proc

echo "========== 测试完成 ==========" | tee -a $RESULT_LOG
echo "所有结果已保存到：$RESULT_LOG"
