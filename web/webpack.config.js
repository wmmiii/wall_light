const CopyPlugin = require("copy-webpack-plugin");
const RemovePlugin = require('remove-files-webpack-plugin');
const VueLoaderPlugin = require('vue-loader/lib/plugin');
const path = require('path');

module.exports = {
  entry: './src/index.ts',
  module: {
    rules: [
      {
        test: /\.scss$/,
        use: [
          'vue-style-loader',
          {
            loader: 'css-loader',
            options: {
              esModule: false
            }
          },
          'sass-loader',
        ]
      },
      {
        test: /\.tsx?$/,
        loader: 'ts-loader',
        exclude: /node_modules/,
        options: {
          appendTsSuffixTo: [/\.vue$/],
        }
      },
      {
        test: /\.vue$/,
        loader: 'vue-loader',
      },
    ],
  },
  resolve: {
    extensions: [ '.tsx', '.ts', '.js', '.vue' ],
  },
  output: {
    filename: 'index.js',
    path: path.resolve(__dirname, '../docs'),
  },
  plugins: [
    new VueLoaderPlugin(),
    new RemovePlugin({
      before: {
        allowRootAndOutside: true,
        include: [
          '../docs',
        ],
      },
    }),
    new CopyPlugin({
      patterns: [
        { from: "static", to: "../docs" },
      ],
    }),
  ],
};