#!/bin/bash
CXX=g++
CXXFLAGS="-O2 -pthread"
PROGRAMS=("single" "thread" "proc")
SIZES=(1 10 100 1024 2048 3072 4096)
RESULT_FILE="result.log"
THREADS=4
TASKS=100000

> $RESULT_FILE

echo "========== 编译阶段 =========="
for prog in "${PROGRAMS[@]}"; do
    echo "正在编译 $prog.cpp ..."
    $CXX $CXXFLAGS -o $prog $prog.cpp
done

echo "========== 数据文件生成阶段 =========="
for size in "${SIZES[@]}"; do
    FILE="data_${size}M.bin"
    if [ ! -f "$FILE" ]; then
        echo "生成 $FILE ..."
        dd if=/dev/urandom of=$FILE bs=1M count=$size status=none
    fi
done

echo "========== 性能测试阶段 =========="
for prog in "${PROGRAMS[@]}"; do
    echo "---- 测试程序：$prog ----" | tee -a $RESULT_FILE
    for size in "${SIZES[@]}"; do
        FILE="data_${size}M.bin"
        echo -n "[$FILE] " | tee -a $RESULT_FILE

        # 根据不同程序传入不同参数
        case $prog in
            single)
                { /usr/bin/time -f "%e 秒" ./$prog $FILE output_${size}M.txt 1>/dev/null; } 2>&1 | tee -a $RESULT_FILE
                ;;
            thread)
                { /usr/bin/time -f "%e 秒" ./$prog $FILE $THREADS $TASKS 1>/dev/null; } 2>&1 | tee -a $RESULT_FILE
                ;;
            proc)
                { /usr/bin/time -f "%e 秒" ./$prog $FILE $THREADS $TASKS 1>/dev/null; } 2>&1 | tee -a $RESULT_FILE
                ;;
        esac
    done
    echo "" | tee -a $RESULT_FILE
done

echo "========== 测试完成 =========="
echo "结果已保存到 $RESULT_FILE"
