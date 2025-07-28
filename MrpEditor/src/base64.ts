/**
 * 将二进制数据编码为Base64字符串
 * @param data 二进制数据（Uint8Array）
 * @returns Base64编码的字符串
 */
export function encodeBase64(data: Uint8Array): string {
    if (!data) throw new Error("Input data cannot be null");
  // 在浏览器环境中使用btoa，在Node.js中使用Buffer
//   if (typeof window !== 'undefined' && typeof window.btoa === 'function') {
    // 浏览器环境
    let binary = '';
    const len = data.byteLength;
    for (let i = 0; i < len; i++) {
      binary += String.fromCharCode(data[i]);
    }
    return window.btoa(binary);
//   } else {
//     // Node.js环境
//     return Buffer.from(data).toString('base64');
//   }
}

/**
 * 将Base64字符串解码为二进制数据
 * @param base64Str Base64编码的字符串
 * @returns 解码后的二进制数据（Uint8Array）
 */
export function decodeBase64(base64Str: string): Uint8Array {
     if (!base64Str) throw new Error("Input string cannot be null");
  // 在浏览器环境中使用atob，在Node.js中使用Buffer
//   if (typeof window !== 'undefined' && typeof window.atob === 'function') {
    // 浏览器环境
    const binaryStr = window.atob(base64Str);
    const len = binaryStr.length;
    const bytes = new Uint8Array(len);
    for (let i = 0; i < len; i++) {
      bytes[i] = binaryStr.charCodeAt(i);
    }
    return bytes;
//   } else {
//     // Node.js环境
//     return new Uint8Array(Buffer.from(base64Str, 'base64'));
//   }
}