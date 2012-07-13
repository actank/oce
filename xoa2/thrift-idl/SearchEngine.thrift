struct SearchResult {
  1: list<string> seg_words,
  2: list<i32> article_ids
}

service SearchEngine {
  SearchResult Search(1: string text, 2: i32 start, 3: i32 count)
}
