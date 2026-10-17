/**
 * @file deflate256.h
 * @brief 組み込みシステム向け極小DEFLATE（ZIP）圧縮・解凍モジュール
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @details 
 * 本モジュールは、リソースが極めて制限された8/16/32ビットマイコン向けに設計された、
 * 超軽量のDEFLATE規格（RFC 1951）準拠エンコーダおよびデコーダです。
 *
 * @section constraints 動作パラメーター制限（PC側での圧縮条件）
 * 本モジュールが動作するためには、圧縮データが以下の条件を満たしている必要があります。
 * - **スライディングウィンドウサイズ**: 最大256バイトに固定（2^8バイト）。
 * - **ハフマン符号化方式**: 固定ハフマン符号（Fixed Huffman）限定。動的ハフマン木は非対応。
 * - **ヘッダーフォーマット**: zlib/GZIP/ZIP等のヘッダーやフッターを含まない「Raw DEFLATE」形式。
 *
 * @section memory_specs メモリ仕様（メモリ消費量の極小化）
 * - **解凍（デコーダ）**: 独立したワークRAM消費は**完全に0バイト**です。
 *   解凍データを順次格納していく出力バッファ配列そのものを、過去256バイトの履歴参照（ウィンドウ）
 *   として直接兼用する設計になっています。
 * - **圧縮（エンコーダ）**: 文字列検索用のハッシュテーブル（通常数KB〜数十KB）を完全に排除。
 *   直近256バイトの履歴を保持する固定配列（**わずか256バイト**）のRAMのみで動作します。
 * - **動的メモリ**: 圧縮・解凍ともに `malloc()` 等の動的メモリ割り当ては一切行いません。
 */

#ifndef DEFLATE256_H
#define DEFLATE256_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 元データをRaw DEFLATE（256B窓・固定ハフマン）形式へ圧縮します。
 *
 * @details
 * 入力された生データから、過去256バイトの履歴（リングバッファ）を用いて最長一致文字列（LZ77）を
 * 高速に検索し、DEFLATE規格に準拠した固定ハフマン符号のビットストリームを出力バッファへパッキングします。
 *
 * @param[in]  src           圧縮対象の元データ（生データ）配列へのポインタ。
 * @param[in]  src_len       元データのバイトサイズ。
 * @param[out] dst           圧縮データ（ビットストリーム）を出力するバッファへのポインタ。
 * @param[in]  dst_max_len   出力先バッファの最大許容サイズ（バッファオーバーラン防止用）。
 *
 * @return uint32_t 実際に生成された圧縮データのバイトサイズ。
 *                  出力バッファが不足した場合、または内部エラーが発生した場合は 0 を返します。
 *
 * @pre @p src および @p dst は有効なメモリ領域を指している必要があります。
 * @note 圧縮履歴の保持に、関数内部で256バイトのスタック領域（ローカル配列）を消費します。
 */
uint32_t deflate256_encode(const uint8_t *src, uint32_t src_len, uint8_t *dst, uint32_t dst_max_len);

/**
 * @brief 制限付きのRaw DEFLATEデータを元のデータへと解凍（展開）します。
 *
 * @details
 * 独立したスライディングウィンドウ用のバッファを持たず、解凍データを書き込んでいく
 * @p dst 配列そのものを過去の履歴（ウィンドウ）として直接参照して文字列コピーを行います。
 *
 * @param[in]  src       圧縮されたRaw DEFLATEデータ配列へのポインタ。
 * @param[in]  src_len   圧縮データのバイトサイズ。
 * @param[out] dst       解凍されたデータを格納する出力先配列へのポインタ。
 *                       （過去256バイトの重複参照を行うため、RAM上の書き換え可能領域である必要があります）
 *
 * @return int 解凍処理の結果ステータス。
 * @retval 0   解凍成功。
 * @retval -1  入力ビットストリームの異常、または不正なハフマン符号を検出（解凍失敗）。
 *
 * @warning @p dst バッファは、過去のデータを読み出す履歴ウィンドウを兼ねているため、
 *          Flash ROMなどの読み出し専用領域や、ストリーミング出力（シリアルポートへ直接投げる等）
 *          に設定することはできません。必ず十分なサイズを持つRAM配列を指定してください。
 */
int deflate256_decode(const uint8_t *src, uint32_t src_len, uint8_t *dst);

#ifdef __cplusplus
}
#endif

#endif /* DEFLATE256_H */]