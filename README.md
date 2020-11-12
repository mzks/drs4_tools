# drs4_tools
DRS4 evaluation boardを使うためのツール類
 - [公式サイト](https://www.psi.ch/en/drs/evaluation-board)

## リポジトリに含まれるもの
 - 公式ツールのインストールガイド
 - DRS4 evaluation boardを物理測定に使うTips
 - 公式オシロで取得したバイナリデータをROOT TTree に変換するデコーダー
 - CUI DAQ Script

## 公式ツールをふくめたインストール
### Prerequirement 
MacOS (後述), wxWidgets, libusb.
```
$ brew install libusb wxmac
```

### Build
```
$ git clone git@github.com:mzks/drs4_tools.git
$ cd drs4_tools/build
$ ./build.sh
```
公式ツールをダウンロードし,小さなバグを修正し,CUIツールと一緒にビルドします.
`drs4_tools/daq` が作成されます.

 - `daq/drsosc` : 公式のオシロスコープアプリケーション
 - `daq/drscl`  : 公式のCUI Configure ツール
 - `daq/drscmd` : CUI DAQ ツール

## つかいかた

### `daq/drsosc`

公式のオシロスコープアプリケーションで, 直感的な操作ができます.
Config ボタンからVoltage Calibration と Timing Calibration を行う必要があります.
Voltage Calibration はチェックボックスを上からひとつずつ Checkして何度もするとベースラインノイズが低減するきがします.
Sampling rateを変更する際は Formに入力した際にEnterキーをおす必要があります.

Saveボタンからbinary形式 (*.dat)でかきだしたWaveformファイルは`decoder/decoder.C`でROOT treeにデコードすることができます.

### `daq/drscl`
公式のCUIコントロールで, `help` コマンドで全コマンド参照できます.


### `daq/drscmd`
CUI DAQ ツール.
