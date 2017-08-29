
#ifndef _FreeImageCodec_H__
#define _FreeImageCodec_H__

#include "BWImageCodec.h"
#include "BWDataStream.h"
#include <list>
struct FIBITMAP;

	class  FreeImageCodec : public ImageCodec
    {
    private:
        std::string mType;
        unsigned int mFreeImageType;

		typedef std::list<ImageCodec*> RegisteredCodecList;
		static RegisteredCodecList msCodecList;

		/** Common encoding routine. */
		FIBITMAP* encode(MemoryDataStreamPtr& input, BWCodecDataPrt& pData) const;

    public:
        FreeImageCodec(const std::string &type, unsigned int fiType);
        virtual ~FreeImageCodec() { }

        /// @copydoc Codec::code
		BWDataStreamPrt code(MemoryDataStreamPtr& input, BWCodecDataPrt& pData) const;
        /// @copydoc Codec::codeToFile
		void codeToFile(MemoryDataStreamPtr& input, const std::string& outFileName, BWCodecDataPrt& pData) const;
        /// @copydoc Codec::decode
		DecodeResult decode(BWDataStreamPrt& input) const;

        
        virtual std::string getType() const;        

		std::string magicNumberToFileExt(const char *magicNumberPtr, size_t maxbytes) const;

		static void startup(void);
		static void shutdown(void);
    };


#endif
