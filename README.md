# stsender

(sound teleporter: sender)

libcomposite で作成した偽の USB-DAC の音を、ゲームに支障がない程度の低遅延でネットワーク越しに送信するプログラムです。

## 必要なもの

* PC側の受信プログラム
  * Mac版は現在制作中です (未公開)。
* USB デバイスになりながら (libcomposite が使える) PCとのIP通信ができる Raspberry Pi
  * Raspberry Pi Zero W(H) で動作確認済みです
  * Raspberry Pi Zero 無印 は GPIO でがんばるとできるかも？
  * Raspberry Pi 4 は Ethernet ポートがあるので安定すると思います
    * が、電源供給がネックかも？
  * まあ別に libcomposite が使えればそもそも Raspberry Pi でなくてもいいと思います
* これ

## 想定

```
ボイスチャット (Discordなど)
↓ (Internet)
(PC w/ 受信プログラム) → ヘッドセット → あなた
↑ Ethernet (Wi-Fi is not recommended)
(LAN... or probably Wi-Fi Direct or something else?)
↑ Ethernet or Wi-Fi
(Raspberry Pi w/ stsender) ← 電源供給
↑ USB
ゲーム機 (Nintendo Switch など)
```

## TODO

- ポート可変にする
  - 現状 12345/tcp で固定
- Avahi に サービスを登録
  - ついでにチャンネル数とかサンプリングレートも送る
    - ………でもそれらの値って実はデバイスを次に開くまでに変わりうるのでは？
- なんか死ぬじゃなくてちゃんと原因を探る

## LICENSE

MIT License.