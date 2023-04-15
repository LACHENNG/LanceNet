#include <LanceNet/base/StringPiece.h>
namespace LanceNet
{

StringPiece::StringPiece()
    : ptr_(NULL), length_(0) { }

StringPiece::StringPiece(const char* str)
    : ptr_(str), length_(static_cast<int>(strlen(ptr_))) { }

StringPiece::StringPiece(const unsigned char* str)
    : ptr_(reinterpret_cast<const char*>(str)),
    length_(static_cast<int>(strlen(ptr_))) { }

StringPiece::StringPiece(const std::string& str)
    : ptr_(str.data()), length_(static_cast<int>(str.size())) { }

StringPiece::StringPiece(const char* offset, int len)
    : ptr_(offset), length_(len) { }

const char* StringPiece::data() const { return ptr_; }
int StringPiece::size() const { return length_; }
bool StringPiece::empty() const { return length_ == 0; }
const char* StringPiece::begin() const { return ptr_; }
const char* StringPiece::end() const { return ptr_ + length_; }

void StringPiece::clear() { ptr_ = NULL; length_ = 0; }

void StringPiece::set(const char* buffer, int len) {
    ptr_ = buffer; length_ = len;
}

void StringPiece::set(const char* str) {
    ptr_ = str;
    length_ = static_cast<int>(strlen(str));
}
void StringPiece::set(const void* buffer, int len) {
    ptr_ = reinterpret_cast<const char*>(buffer);
    length_ = len;
}

char StringPiece::operator[](int i) const { return ptr_[i]; }

void StringPiece::remove_prefix(int n) {
    ptr_ += n;
    length_ -= n;
}

void StringPiece::remove_suffix(int n) {
    length_ -= n;
}

bool StringPiece::operator==(const StringPiece& x) const {
    return ((length_ == x.length_) &&
            (memcmp(ptr_, x.ptr_, length_) == 0));
}
bool StringPiece::operator!=(const StringPiece& x) const {
    return !(*this == x);
}


int StringPiece::compare(const StringPiece& x) const {
    int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_);
    if (r == 0) {
        if (length_ < x.length_) r = -1;
        else if (length_ > x.length_) r = +1;
    }
    return r;
}

std::string StringPiece::as_string() const {
    // very likly that is auto RVO by compiler
    return std::string(data(), size());
}

void StringPiece::CopyToString(std::string* target) const {
    target->assign(ptr_, length_);
}

// Does "this" start with "x"
bool StringPiece::starts_with(const StringPiece& x) const {
    return ((length_ >= x.length_) && (memcmp(ptr_, x.ptr_, x.length_) == 0));
}

}  // namespace LanceNet

// allow StringPiece to be logged
std::ostream& operator<<(std::ostream& os, const LanceNet::StringPiece& piece)
{
    os << piece.as_string();
    return os;
}

