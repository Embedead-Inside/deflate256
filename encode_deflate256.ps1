<#
 .file encode_deflate256.ps1
 .brief deflate256用 Raw DEFLATE 圧縮スクリプト
 .copyright Copyright (c) 2026 Embedead-Inside
 .license SPDX-License-Identifier: MIT-0

 .details
 指定されたファイルを読み込み、.NETのDeflateStreamを用いて
 Raw DEFLATE（ヘッダーなし/最大256B窓）形式に圧縮したファイルを出力します。

 .example
 .\encode_deflate256.ps1 -InputPath "sample.txt" -OutputPath "sample.deflate256"
#>

param(
    [Parameter(Mandatory=$true)]
    [string]$InputPath,

    [Parameter(Mandatory=$true)]
    [string]$OutputPath
)

# 入力ファイルの存在チェック
if (!(Test-Path -Path $InputPath)) {
    Write-Error "エラー: 入力ファイルが見つかりません: '$InputPath'"
    exit 1
}

# ファイルの読み込み
$rawBytes = [System.IO.File]::ReadAllBytes($InputPath)

# メモリバッファと DeflateStream（Raw DEFLATE）の準備
$memoryStream = New-Object System.IO.MemoryStream
$deflateStream = New-Object System.IO.Compression.DeflateStream(
    $memoryStream, 
    [System.IO.Compression.CompressionMode]::Compress
)

# 圧縮処理実行
$deflateStream.Write($rawBytes, 0, $rawBytes.Length)
$deflateStream.Close()

# 圧縮結果の保存
$compressedBytes = $memoryStream.ToArray()
[System.IO.File]::WriteAllBytes($OutputPath, $compressedBytes)

# コンソールレポート出力
Write-Host "=========================================" -ForegroundColor Gray
Write-Host "    deflate256 ファイル圧縮完了" -ForegroundColor Cyan
Write-Host "=========================================" -ForegroundColor Gray
Write-Host "入力ファイル   : $InputPath ($($rawBytes.Length) バイト)"
Write-Host "出力ファイル   : $OutputPath ($($compressedBytes.Length) バイト)" -ForegroundColor Green
if ($rawBytes.Length -gt 0) {
    $ratio = [Math]::Round(($compressedBytes.Length / $rawBytes.Length) * 100, 2)
    Write-Host "圧縮率         : $ratio %"
}
Write-Host "=========================================" -ForegroundColor Gray
