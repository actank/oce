package com.xiaonei.search3.model;

import java.util.List;

import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;

/**
 * 每个实现类自己实现如何将数据转变为Document对象以及Fields对象:
 * <p>
 * toStringBuffer()
 * </p>
 * <p>
 * toDocuments()
 * </p>
 * <p>
 * toFields()
 * </p>
 **/
interface Indexable {
	StringBuffer toStringBuffer();

	List<Document> toDocuments();

	List<Field> toFields();
}
