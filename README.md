# Tiny View Plus Practice

practice.png

Tiny View Plus Practiceは、Tiny View PlusのARマーカを利用したタイム計測表示を強化した練習会用のアプリです。

![tinyviewpluspractice](docs/practice.png)

<font color="crimson">ここに記載していない部分は　Tiny View Plus を参照してください。</font>

[https://github.com/t-asano/tinyviewplus](https://github.com/t-asano/tinyviewplus)


## 動作環境

<table>
<tr>
<td>OS</td><td>Windows (10 64bit推奨)</td>
</tr>
<tr>
<td>受信機</td><td>Eachine ROTG02<br/>Eachine ROTG01</td>
</tr>
</table>


## インストール

### ビルド済みバイナリを使用する場合(通常はこちら)

1. [リリースページ](https://github.com/chibaron/tinyviewpluspractice/releases)よりzipファイルをダウンロード
2. zipファイルを展開して適当な場所に配置

バイナリの作成にあたっては細心の注意を払っていますが、アプリを実行することで起こる不具合については、一切責任を負いません。


## 使い方

### キー操作

| キー | 機能 | 初期値 |
|---|---|---|
| H | ヘルプの表示 | - |
| A | 基準タイム設定 | 10秒 |
| M | 最小ラップタイムの設定(1~100秒) | 3秒 |
| S | セッションタイムアウトの設定 | 120秒 ※|
| Tab | 項目選択 | - |
| Up | 項目内スクロール | - |
| Down | 項目内スクロール| - |
| Left | 項目内スクロール | - |
| Right| 項目内スクロール | - |
| . | アプリの終了 | - |

※ マーカーを検出しない場合は初期パイロットに戻ります。


### マウス操作

現在は使用できません。(将来対応予定)

### ラップ表示

| 種別　| 詳細 |
|---|---|
| bestlap | 各パイロット毎の1周の最速タイムです。 赤で表示されます。|
| best3lap | 各パイロット毎の連続した３週の最速タイムです。青で表示されます。|
| 基準タイム | 基準タイム以下の場合は緑で表示されます。|


### QR code reader

QRコードを使用してパイロットの識別を行います。

マニュアルでの入力はできません。

QRコードをOSDに組み込んでおくと便利です。以下は、Betaflight用のロゴ画像の例です。

![docs/qr_betaflight.png](docs/qr_betaflight.png)



## License

Tiny View Plus is distributed under the MIT License. This gives everyone the freedoms to use Tiny View Plus in any context: commercial or non-commercial, public or private, open or closed source. Please see [LICENSE.md](LICENSE.md) and [LICENSE\_THIRD\_PARTY.md](LICENSE_THIRD_PARTY.md) for details.
