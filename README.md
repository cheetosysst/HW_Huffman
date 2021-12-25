# HW_Huffman

這個作業寫了三個禮拜，過程異常困難，連助教都想問我為什麼可以把作業複雜畫到那麼誇張。我也不知道為什麼一個看似簡單的作業我可以稿的這麼複雜，重寫了三次，一共耗時三個禮拜。

## 作業題目
利用 Huffman tree 對一個文字檔進行壓縮，輸出成二進位的檔案。

## 實做方式
單純的 linked list 實做的 priority queue、singly linked list、binary tree

## 檔案
- `huffman_old_old.c`
	第一次嘗試，對於赫夫曼的概念有誤，加上檔案過於混亂，最後放棄。
 
- `huffmac_old.c`
	第二次嘗試，對赫夫曼的理解依舊有誤，但是在實做細節上有改進，最後因為檔案行數過多、過於混亂放棄。

- `huffman.c` & 其他標頭檔
	為方便撰寫，決定分檔案個別寫每個結構。重新理解了 huffman 原理，所以這一次的樹應該是正確的（前兩次嘗試不但沒有壓縮到，檔案大小還增加了）。

- Usage
```bash
touch input.txt
make && make test
```
執行結果分別是：
- `code.txt`
	ascii 和 二進制對照表
- `compress`
	壓縮過的文章
- `output.txt`
	解壓縮後的文章

我是不是真的不會寫 quq