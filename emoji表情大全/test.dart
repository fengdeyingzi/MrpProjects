import 'dart:convert';
import 'dart:io';

void main() async {
  try {
    // 指定文件路径
    const fileName = 'assets/emoji.txt';
    
    // 创建文件对象
    final file = File(fileName);
    
    // 检查文件是否存在
    if (!await file.exists()) {
      print('错误：文件 $fileName 不存在');
      return;
    }
    
    // 以 UTF-8 编码读取文件内容
    final content = await file.readAsString(encoding: utf8);
    
    // 打印到控制台
    print('文件内容：');
    print('-' * 40);
    print(content);
    print('-' * 40);
    
    // 可选：显示字符数（包含emoji）
    print('字符数：${content.length}');
    
  } catch (e) {
    // 处理可能的错误
    print('读取文件时发生错误：$e');
  }
}