const path = require('path');

module.exports = {
  optimization: {
    minimize: false  // 完全禁用压缩
  },
  entry: {
    Map565Viewer: './src/Map565Viewer.ts'
  },
  module: {
    rules: [
      {
        test: /\.tsx?$/,
        use: 'ts-loader',
        exclude: /node_modules/,
      },
    ],
  },
  resolve: {
    extensions: ['.tsx', '.ts', '.js'],
  },
  output: {
    filename: '[name].bundle.js',
    path: path.resolve(__dirname, 'dist'),
  },
};
