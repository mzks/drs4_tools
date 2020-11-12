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
[公式ドキュメント](https://www.psi.ch/sites/default/files/2020-02/manual_rev51.pdf)
### `daq/drsosc`

公式のオシロスコープアプリケーションで, 直感的な操作ができます.
Config ボタンからVoltage Calibration と Timing Calibration を行う必要があります.
Voltage Calibration はチェックボックスを上からひとつずつ Checkして何度もするとベースラインノイズが低減するきがします.
Sampling rateを変更する際は Formに入力した際にEnterキーをおす必要があります.

Saveボタンからbinary形式 (*.dat)で書き出したWaveformファイルは`decoder/decoder.C`でROOT tree形式にデコードすることができます.

|Branch Name |説明 |
| ---------- | --------------------------- |
|eventSerial |ファイルでユニークなイベント番号 |
|Year        |イベントの取得時間 (年)        |
|Month       |月 |
|Day         |日 |
|Hour        |時間 |
|Minute      |分 |
|Second      |秒 |
|MillSecond  |ミリ秒 |
|Range       |電圧範囲 Range-0.5V -- Range+0.5V |
|triggerSell |トリガーチャンネル|
|ch1wf       |ch1 のWaveform [1024]|
|ch2wf       | |
|wf          |Waveform [4[1024]]|
|ch1Scaler   |スケーラー |
|ch2Scaler   | |


### `daq/drscl`
公式のCUIコントロールで, `help` コマンドで全コマンド参照できます.

`./drscl`
```
B0> help
Available commands:

active <0|1>                Set domino active mode on (1) or off (0)
board <i>|<i1> <i2>|all     Address individual board/range/all boards
calib [dir]                 Response Calibration. Use dir="area" for MEG
chn [n]                     Set number of channels: 8, 4, 2, 1
ct                          Chip Test
del <0|1>                   Switch delayed start on/off
dir                         Show CF directory
dmode <0|1>                 Set Domino mode 0=single, 1=cont.
et                          EEPROM test
exit                        Exit program
freq <ghz> [0|1]            Set frequency of board [without/with] regulation
info                        Show information about board
init                        Initialize board
led <0|1>                   Turn LED on (1) or off (0)
lock [0|1]                  Display lock status [without/with] restart
multi [0|1]                 Turn multi-buffer mode on/off
offset <voltage>            Set offset voltage
phase <value> [0|1]         Set ADC clock phase and inversion
quit                        Exit program
ram                         Test speed to FPGA RAM
range <center>              Change input range to <center>+=0.5V
read <chn> [0|1] [file]     Read waveform to [file], chn=0..19 [with] calibration
refclk [0|1]                Use FPGA ref. clk (0) or ext. P2 ref. clk (1)
reg                         Register test
serial <number>             Set serial number of board
scan                        Scan for boards
standby <0|1>               Turn standby mode on (1) or off (0)
start                       Start domino wave
stop                        Issue soft trigger
tcout [file] [idx_offset]   Print time calibration of DRS4, or write it onto [file]
tcs <0|1>                   Timing calibration signal on (1) or off (0)
tcalib [freq]               Timing Calibration
tlevel <voltage>            Set trigger level in Volts
trans <0|1>                 Set transparent mode on (1) or off (0)
trig <0|1>                  Hardware trigger on (1) or off (0)
upload <file>               Upload ACE file to CF
volt off|<voltage>          Turn calibration voltage on/off
```
rangeを変更する際は, `range`コマンドで変更したのち, `calib`コマンドで再度変更する必要があります.
frequencyを変更する際は, `freq`コマンドで変更したのち, `tcalib`コマンドで再度変更する必要があります.

### `daq/drscmd`
CUI DAQ ツール.
```
./drscmd --help
usage: ./drscmd [options] ...
options:
  -l, --datadir       Data directory (string [=/Users/mzks/drs4/data/])
  -f, --filename      Output file name (string [=test])
  -n, --num_of_evt    Number of events (unsigned long [=10])
  -c, --trigger_ch    Trigger channel (1--4) 0 is ext. (int [=1])
  -e, --edge_neg      Trigger type (negative or positive) (bool [=1])
  -t, --trigger       Trigger level (V) (double [=0])
  -u, --neg_trig      Negative trigger level
  -d, --delay         Trigger delay (ns) (int [=0])
  -s, --sampling      Sampling Frequency (GHz) (double [=1])
  -i, --center        Input range Center (V) (double [=0])
  -r, --reset         DRS4 Reset without data taking
  -?, --help          print this message
```
ここで指定するcenter, samplingは前述の`calib`, `tcalib`コマンドで予め信号を外した状態でCalibration しておく必要があります.
負値のトリガーを徹底する際は, `-u -t 0.3`などと設定すると, -0.3 Vのトリガースレッショルドを設定できます.
出力はASCIIで, 1000イベントごとに, `<filename>.000n.dat`のようにファイルが作られます.
また, `<filename>.txt`に実行したConfigurationが保存されます.
このASCIIファイルをROOT tree形式に変換するマクロは`decoder/converter.C`です.
ただしこちらにつくTimestampはシステムのTimestampで多少の誤差があります.

### 注意点
DRS4に過剰な電流を流すと壊れるので注意してください.
