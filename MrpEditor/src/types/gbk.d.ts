declare module 'gbk.js' {
  export function encode(str: string): Uint8Array;
  export function decode(bytes: Uint8Array): string;
}
