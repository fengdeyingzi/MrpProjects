const path = require('path');

module.exports = {
  optimization: {
    minimize: false  // 完全禁用压缩
  },
  entry: {
    mrpEditor: './src/mrp-editor.ts'
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
    alias: {
      'midi.js': path.resolve(__dirname, 'src//midi.js')
    }
  },

  output: {
    filename: '[name].bundle.js',
    path: path.resolve(__dirname, 'dist'),
  },
};
