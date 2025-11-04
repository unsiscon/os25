#!/bin/bash
# ==========================================
# 实验二：多进程/多线程文件读取与排序 自动测试脚本
# 环境：Ubuntu / WSL，gcc/g++ >= 9.4，支持 C++17
# ==========================================

# ==== 参数设置 ====
DATA_DIR="./data_files"
LOG="./result.log"
THREADS=4
PROCS=4
SIZES=("1M" "10M" "100M" "1G" "2G")

# ==== 初始化 ====
mkdir -p "$DATA_DIR"
echo "========== 编译阶段 ==========" | tee "$LOG"

echo "正在编译 single.cpp ..."
g++ -std=c++17 -O2 single.cpp -o single

echo "正在编译 thread.cpp ..."
g++ -std=c++17 -O2 -pthread thread.cpp -o thread

echo "正在编译 proc.cpp ..."
g++ -std=c++17 -O2 proc.cpp -o proc

# ==== 数据文件生成 ====
echo "========== 数据文件生成阶段 ==========" | tee -a "$LOG"
for size in "${SIZES[@]}"; do
    file="$DATA_DIR/data_${size}.bin"
    if [ ! -f "$file" ]; then
        echo "生成 $file ..."
        dd if=/dev/urandom of="$file" bs=$size count=1 status=progress
    else
        echo "$file 已存在，跳过生成。"
    fi
done

# ==== 测试函数 ====
run_test() {
    exe=$1
    shift
    echo "---- 测试程序：$exe ----" | tee -a "$LOG"
    for size in "${SIZES[@]}"; do
        file="$DATA_DIR/data_${size}.bin"
        if [ ! -f "$file" ]; then
            echo "❌ 文件 $file 不存在，跳过。" | tee -a "$LOG"
            continue
        fi

        # 获取文件大小（字节），计算整型元素个数
        bytes=$(stat -c%s "$file")
        total_items=$((bytes / 4))
        echo "[data_${size}.bin]" | tee -a "$LOG"

        start=$(date +%s.%N)

        case $exe in
            single)
                ./single "$file" "output_${size}_single.txt" ;;
            thread)
                ./thread "$file" $THREADS $total_items "output_${size}_thread.txt" ;;
            proc)
                ./proc "$file" $PROCS $total_items "output_${size}_proc.txt" ;;
        esac

        end=$(date +%s.%N)
        runtime=$(awk -v s=$start -v e=$end 'BEGIN {printf "%.2f", e - s}')
        echo "耗时: $runtime 秒" | tee -a "$LOG"
    done
    echo "" | tee -a "$LOG"
}

# ==== 性能测试 ====
echo "========== 性能测试阶段 ==========" | tee -a "$LOG"
run_test "single"
run_test "thread"
run_test "proc"

echo "========== 测试完成 ==========" | tee -a "$LOG"
echo "结果已保存到 $LOG"
