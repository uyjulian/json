/**
 * 出力処理用インターフェース
 */
class IWriter {
protected:
	const tjs_char *newlinestr;
public:
	int indent;
        bool hex;
	IWriter(int newlinetype=0) {
		indent = 0;
                hex = false;
		switch (newlinetype) {
		case 1:
			newlinestr = TJS_W("\n");
			break;
		default:
			newlinestr  = TJS_W("\r\n");
			break;
		}
	}
	virtual ~IWriter(){};
	virtual void write(const tjs_char *str) = 0;
	virtual void write(tjs_char ch) = 0;
	virtual void write(tTVReal) = 0;
	virtual void write(tTVInteger) = 0;

	inline void newline() {
		write(newlinestr);
		for (int i=0;i<indent;i++) {
			write((tjs_char)' ');
		}
	}

	inline void addIndent() {
		indent++;
		newline();
	}

	inline void delIndent() {
		indent--;
		newline();
	}
};

/**
 * 文字列出力
 */
class IStringWriter : public IWriter {

public:
	ttstr buf;
	/**
	 * コンストラクタ
	 */
	IStringWriter(int newlinetype=0) : IWriter(newlinetype) {};

	virtual void write(const tjs_char *str) {
		buf += str;
	}

	virtual void write(tjs_char ch) {
		buf += ch;
	}

	virtual void write(tTVReal num) {
               if (hex) {
                 tTJSVariantString *str = TJSRealToHexString(num);
                 buf += str;
                 str->Release();
                 buf += TJS_W(" /* ");
                 str = TJSRealToString(num);
                 buf += str;
                 str->Release();
                 buf += TJS_W(" */");
               } else {
                 tTJSVariantString *str = TJSRealToString(num);
                 buf += str;
                 str->Release();
               }
	}

	virtual void write(tTVInteger num) {
		tTJSVariantString *str = TJSIntegerToString(num);
		buf += str;
		str->Release();
	}
};

/**
 * ファイル出力
 */
class IFileWriter : public IWriter {

	/// 出力バッファ
	ttstr buf;
	/// 出力ストリーム
#if 1
	IStream *stream;
#else
	iTJSBinaryStream *stream;
#endif
	bool utf;
	char *dat;
	int datlen;
	
public:

	/**
	 * コンストラクタ
	 */
	IFileWriter(const tjs_char *filename, bool utf=false, int newlinetype=0) : IWriter(newlinetype) {
#if 1
		stream = TVPCreateIStream(filename, TJS_BS_WRITE);
#else
		stream = TVPCreateStream(filename, TJS_BS_WRITE);
#endif
		this->utf = utf;
		dat = NULL;
		datlen = 0;
	}

	/**
	 * デストラクタ
	 */
	~IFileWriter() {
		if (stream) {
			if (buf.length() > 0) {
				output();
			}
#if 1
			stream->Commit(STGC_DEFAULT);
			stream->Release();
#else
			//stream->Commit(STGC_DEFAULT);
			stream->Destruct();
#endif
		}
		if (dat) {
			free(dat);
		}
	}
	
	void output() {
		if (stream) {
			ULONG s;
			if (utf) {
				// UTF-8 で出力
				int maxlen = buf.length() * 6 + 1;
				if (maxlen > datlen) {
					datlen = maxlen;
					dat = (char*)realloc(dat, datlen);
				}
				if (dat != NULL) {
					int len = TVPWideCharToUtf8String(buf.c_str(), dat);
#if 1
					stream->Write(dat, len, &s);
#else
					s = stream->Write(dat, len);
#endif
				}
			} else {
				// 現在のコードページで出力
				int len = buf.GetNarrowStrLen() + 1;
				if (len > datlen) {
					datlen = len;
					dat = (char*)realloc(dat, datlen);
				}
				if (dat != NULL) {
					buf.ToNarrowStr(dat, len-1);
#if 1
					stream->Write(dat, len-1, &s);
#else
					s = stream->Write(dat, len-1);
#endif
				}
			}
		}
		buf.Clear();
	}
	
	virtual void write(const tjs_char *str) {
		if (stream) {
			buf += str;
			if (buf.length() >= 1024) {
				output();
			}
		}
	}

	virtual void write(tjs_char ch) {
		buf += ch;
	}

	virtual void write(tTVReal num) {
               if (hex) {
                 tTJSVariantString *str = TJSRealToHexString(num);
                 buf += str;
                 str->Release();
                 buf += TJS_W(" /* ");
                 str = TJSRealToString(num);
                 buf += str;
                 str->Release();
                 buf += TJS_W(" */");
               } else {
                 tTJSVariantString *str = TJSRealToString(num);
                 buf += str;
                 str->Release();
               }
	}

	virtual void write(tTVInteger num) {
		tTJSVariantString *str = TJSIntegerToString(num);
		buf += str;
		str->Release();
	}
};
