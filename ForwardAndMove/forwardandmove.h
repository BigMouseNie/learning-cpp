/**
 * 仅阐述一下 std::forward 和 std::move
 */


/**
 * std::remove_reference
 * 类型萃取，移除引用
 * 假设 T 为 int
 */
// 匹配int,返回int
template <typename T>
struct remove_reference { using type = T; };

// 匹配int&,返回int
template <typename T>
struct remove_reference<T&> { using type = T; };

// 匹配int&&,返回int
template <typename T>
struct remove_reference<T&&> { using type = T; };

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;


/**
 * std::forward
 * 利用引用折叠规则
 */
// 适用于T为int和int&,remove_reference<T>::type为int&(适用于arg), 但是T为int时cast<T&&>转化为右值，T为int&时转化为左值(左值引用的右值为左值)
template <typename T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept {
    return static_cast<T&&>(arg);
}

// 适用于T为int&&,remove_reference<T>::type为int&&(适用于arg), 最后转为右值(右值引用的右值引用)
// 实际上这个版本是不需要的，因为上面的版本可以处理int&&的情况，虽然初始int&& arg会绑定到int&,但是T为int&&,根据引用折叠最后依旧返回int&&
// 但是int&无法绑定到int&&，所以该版本无法处理int&情况
template <typename T>
T&& forward(typename std::remove_reference<T>::type&& arg) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value, "Cannot forward an rvalue as an lvalue");
    return static_cast<T&&>(arg);
}


/**
 * std::move
 */
// T&&万能引用，返回值std::remove_reference<T>::type&&保证了cast和返回的是右值
template <typename T>
constexpr typename std::remove_reference<T>::type&& move(T&& arg) noexcept {
    return static_cast<typename std::remove_reference<T>::type&&>(arg);
}
