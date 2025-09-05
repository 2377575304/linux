#!/bin/bash

# ============================================
# Shell脚本语言教程
# ============================================

# 1. 基本输出
echo "欢迎来到Shell脚本语言教程!"
echo "Shell是一种强大的命令行解释器，广泛用于Unix/Linux系统"

# 2. 变量定义与使用
# 定义变量（注意：变量名和等号之间不能有空格）
myname="xiaoyuxuan"
# 使用变量（在变量名前加$符号）
echo "我的名字是: ${myname}"

# 3. 只读变量
# 将变量定义为只读（之后不能修改）
readonly myname

# 4. 字符串操作
# 字符串拼接
echo "我知道你是 "${myname}"!"
# 获取字符串长度
echo "变量myname的长度是: ${#myname}"
# 字符串切片（从位置1开始，取3个字符）
echo "myname的子字符串(1:3)是: ${myname:1:3}"

# 5. 数组
# 定义数组
days=("周一" "周二" "周三" "周四" "周五" "周六" "周日")
# 访问数组元素（索引从0开始）
echo "第一个工作日是: ${days[0]}"
# 获取数组所有元素
echo "所有工作日: ${days[@]}"
# 获取数组长度
echo "一周有 ${#days[@]} 天"

# 6. 条件语句
# if-else语句
age=18
if [ $age -ge 18 ]; then
    echo "你已经成年了"
else
    echo "你还未成年"
fi

# 7. 循环结构
# for循环
echo "使用for循环遍历数组:"
for day in "${days[@]}"; do
    echo "- $day"
done

# while循环
counter=1
echo "使用while循环计数:"
while [ $counter -le 5 ]; do
    echo "计数: $counter"
    counter=$((counter + 1))
done

# 8. 函数
# 定义函数
greet() {
    echo "你好, $1! 今天是$2."
}

# 调用函数
greet "学习者" "周三"

# 9. 获取用户输入
echo "请输入你的名字:"
read username
echo "欢迎, $username!"

# 10. 算术运算
a=10
b=3
echo "$a + $b = $((a + b))"
echo "$a - $b = $((a - b))"
echo "$a * $b = $((a * b))"
echo "$a / $b = $((a / b))"
echo "$a % $b = $((a % b))"

# ============================================
# 教程结束
# ============================================
echo "Shell脚本基础教程已完成!"
echo "继续学习和实践，掌握更多Shell脚本技巧!"



