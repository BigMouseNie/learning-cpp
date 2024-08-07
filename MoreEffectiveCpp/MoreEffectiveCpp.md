# More Effective C++



NOTE: 有几节是略的，可能是因为太老了，或者没什么写的，又或者我实在看不懂，又或者就是懒

​               总结就是前好多节很容易，后面感觉最后结合实际开发经验



## 一、基础议题



### 1：仔细区别 pointers 和 references

````C++
/*
* 这章简单，主要讲了指针和引用的区别，引用必须要有初值且不能为 null，且引用不可以重新复制，
* 如果你发现引用限制了你的程序导致某些功能难以实现，那么请用指针。
*/　

````



### 2：最好使用 C++ 转型操作符

````C++
/* 见 Effective C++ 条款26 */

````



### 3：绝对不要以多态的方式处理数组

````C++
/* 
* 本节可能已经过时，并没有发现书中的问题，其中一个值得讨论的问题是用 delete[] 
* 释放一个由基类指针指向的派生类数组，在析构的时候书中提出指针的偏移量按照基类的大小来偏移导致错误，
* 但是经过我的实践并不会，还有就是基类的析构函数要是虚的，否则就算偏移量正确也会导致局部析构的现象(见Effective * C++ 条款7),下面是我的实践
*/

// 我知道这些类很丑陋，但是不要在意这些细节
class Base{
public:
    int x = 1;
    Base(){
        std::cout<<"Base Constructing"<<std::endl;
    };

    virtual ~Base(){ // 注意将基类的析构设置成 virtual
        std::cout<<"Base Destructing"<<std::endl;
    };
};

class Derived: public Base{
public:
    int y = 2;
    Derived(){
        std::cout<<"Derived Constructing"<<std::endl;
    };

    ~Derived(){
        std::cout<<"Derived Destructing"<<std::endl;
    };
};

void func(Base* pArr,int numElements){
    for(int i=0;i<numElements;++i){
        std::cout<<pArr[i].x;  // 正确的运行推测出指针的偏移是按照派生类的
    }
    return ;
}

int main(){
    Base* pArr = new Derived[5]; // Base 指针指向一个 派生类数组
    func(pArr,5);
    delete[] pArr;
}
````



### 4：非必要不提供 default constructor

````C++
/*
* 本节作者说明了提供 default constructor 的优点和缺点，和如果不提供 default constructor 如何完成
* 提供 default constructor 的一些操作，这个操作主要是在数组的创建上展开的
* (关于默认构造函数的一些知识见 Effective C++ 条款6)
*/

class MyClass{ // 没有提供默认构造函数
public:
    MyClass(int x):_x(x){};
    ~MyClass(){}
private:
    int _x;
};

int main(){
    MyClass arr[10]; // 编译错误，没有默认构造函数
    MyClass* pArr = new MyClass[10]; // 在堆中申请发生同样的问题
}

// 在栈中申请容易解决(如下)，但是怎么在堆中申请呢
MyClass arr[] = {MyClass(1),MyClass(2)};


// 简单申请一个储存指针的数组，然后初始化每一个类，用数组中的指针指向不就行了
MyClass** ppArr = new MyClass*[2]; // 用 typedef MyClass* PMyClass 可读性更强
ppArr[0] = new MyClass(1);
ppArr[1] = new MyClass(2);
std::cout<<ppArr[1]->getX()<<std::endl;
for(int i=0;i<2;++i){
    delete ppArr[i];
}
delete [] ppArr;

/* 
* 有没有发现每一个对象我们都使用了一个额外的指针来指向，有没有其他的办法呢，
* 一个方式是 placement new(更详细的介绍见条款8，Effective C++ 条款52)
*/

int main(){
    void* rawMemory = operator new[](3*sizeof(MyClass)); // 申请原始内存类似于 malloc
    MyClass* pArr = static_cast<MyClass*>(rawMemory);
    
    new (&pArr[0]) MyClass(1);
    new (&pArr[1]) MyClass(12);
    new (&pArr[2]) MyClass(123);

    std::cout<<pArr[2].getX()<<std::endl;

    for(int i=0;i<3;++i){pArr[i].~MyClass();} // 缺点就是需要手动调用析构函数
    operator delete[] (rawMemory); // 类似于 free 
}


/* 
* 如果有默认的构造函数就不用上面如此麻烦了，但是它也有它的麻烦，比如某一个使用某一个对象之前你必须确定它
* 是否真的被初始化，那么你必须去测试它
*/

````





## 二、操作符



### 5：对定制的“类型转换函数”保持警觉

````C++
/* 
* 本节主要说明了一些隐式转化的隐患，以及如何解决，特别是拥有单自变量构造函数的对象(只需要一个参数或者一个参数
* 就可以构造成功，比如其他的参数都有默认)，令人防不胜防，看下面示例
*/



class MyArray{
public:
    MyArray(int size):_size(size){//单自变量构造函数或Rational(int numerator=0,int denominator=1)
        arr = new int[size];
        for(int i=0;i<size;++i){
            arr[i] = 0;
        }
        std::cout<<size<<std::endl;
    };
    ~MyArray(){
        delete [] arr;
    }
    
    int& operator[](int idx){
        return arr[idx];
    }

    int getSize()const{
        return _size;
    }
private:
    int _size;
    int* arr;
};

// 为什么是 non-member funcation (见Effective C++ 26)
bool operator== (const MyArray& lhs,const MyArray& rhs){ // 重载了 ==，比较两个数组是否相等
    return lhs.getSize() == rhs.getSize();
}

int main(){
    MyArray arr1(3);
    MyArray arr2(5);
    if(arr1 == arr2[2]){ // 其实是 arr1[2],但是疏忽了，然而编译器却接受了
        //...
    } 
    return 0;
}

/*
* 编译器首先寻找这样的一个函数 bool operator== (const MyArray& lhs,int rhs)，但是没有，
* 然后找到了这一个 bool operator== (const MyArray& lhs,const MyArray& rhs)，然后调用了一个
* 隐式转换 MyArray(arr2[2]),这非常不符合逻辑，而且它不会报任何错误，下面是解决方法
*/

explicit MyArray(int size); // 最简单的方式就是在其构造函数前加上 explicit，不允许隐式转换


// 另一种方法是增加一个新的类来表示 ArraySize

class ArraySize{
public:
    ArraySize(int size):theSize(size){}; // 默认就是允许隐式转换
    int getSize()const;
private:
    int theSize;
};

MyArray arr1(3); // 如此的调用依然是合法的，因为“3”被隐式转换为一个 ArraySize

/* 如此不合法，因为 arr2[2]不会隐式转换成ArraySize然后再次隐式转换成Array,C++ 不允许大于一次的调用用户定制转换的行为 */
if(arr1 == arr2[2]){...}; 

````



### 6：区别 increment/decrement 操作符的前置和后置形式

````C++
/*
* increment 和 decrement 之间没什么区别，因此我们以 increment 为例中点关注前置和后置的区别
*/

UPInt& UPInt::operator++(){ // 前置
    *this += 1;
    return *this; // 返回的它本身
}

const UPInt& UPInt::operator++(int){ // 后置
    UPInt oldValue = *this;
    *this += 1;
    return oldValue; // 返回的是一个临对象并且是还没有自增的值，用const 限定有效避免了对临时值操作的可能
}

/*
* 所以除非后置是必要的，否则应该选择更具有效率的前置，但是现在的编译器一般会优化
*/

````



### 7：千万不要重载 && ，|| 和逗号操作符

````C++
/* 对于真假表达式 C++ 采用的是骤死式的评估方式 */
// 如果 p 为空就直接返回 false,但是如果你重载 && 或 ||，函数调用语义就会取代骤死式语义
char* p;
if((p!=null)&&(strlen(p)>10)); 
    
// 例如
if(expression1 && expression2);
/*
* 可能会调用下面的其中一种, 它们都是函数语义，当函数调用被执行所有的函数参数必评估完成，并且 C++ 语言规范
* 没有明确定义函数调用中各个参数的评估顺序
*/
if(expression1.operator&&(expression2));
if(operator&&(expression1, expression2));

/*
* 逗号表达式是左侧会先评估，然后右侧，最后整个逗号表达式的结果以右侧的值为代表，因此如果要重载
* 逗号表达式必须模仿，但是你无法执行这些必要的行为
*/
// 总结一下，就是你没必要重载......

````



### 8：了解各种不同意义的 new 和 delete

````C++
// operator new
void* operator new(size_t size); // 返回原始内存，类似于 malloc



// new operator
string* ps = new string("Memory");
// 它产生了下面的代码(伪代码)
void* memory = operator new(sizeof(string)); 
call string::string("Memory") on *memory; // 调用了 constructor
string* ps = static_cast<string*>(memory);



// Placement new(前文条款4 和 Effective C++ 52已有说明)
class Widget{
public:
    Widget(widgetSize);
};
WIdget* constructWidget(void* buffer, int widgetSize){
    // new operator 接受一个额外的参数 buffer 导致隐式调用 operator new
    return new (buffer) Widget(widgetSize); // 返回一个指向构造在 buffer 的 Widget 指针
}



/* 删除与内存释放 */
/* 非常简单，按照栈的方式(先进后出)调用对应的 delete、free 和 destructor */

````





## 三、异常



### 9：利用 destructors 避免泄漏内存

````C++
/* 本小节泄漏资源问题主要讨论如何预防异常发生在 delete 之前 */

void func(){
    MyClass* p = new MyClass;
    doSomething; //发生异常，下面的 delete 无法正常调用
    delete p;
}

// try catch 语句
void func(){
    MyClass* p = new MyClass;
    try{
        doSomething;
    }catc(...){ // 捕捉所有异常
        delete p; // 抛出异常，防止资源泄漏
        throw;
    }
    delete p; // 无异常，正常调用
}


// 智能指针 (见 Effective C++ 13)


// 以对象管理资源(见 Effective C++ 13)

````



### 10：在 constructors 内阻止资源泄漏

````C++
/* 本小节介绍了如果在构造函数中伴随这资源申请，但不幸抛出异常的时如何释放内存 */ 

class Person{
public:
    Person(std::string& name, std::string& address):
    	_name(0),_address(0) // 先初始化为 null
    {
        if(name!=""){_name = new Name(name);}
        if(address!=""){_address = new Address(address);} // 如果这了抛出异常 _name 怎么
    }
private:
    Name* _name; // Name 类
    Address* _age; // Address 类
};


// try catch

Person(std::string& name, std::string& address):
	_name(0),_address(0) // 先初始化为 null
{
    try{
        if(name!=""){_name = new Name(name);}
        if(address!=""){_address = new Address(address);}
    } catch(...){
        delete _name;
        delete _address;
    }
}

// 其他的就不举例了，因为直接用智能指针可以方便直观的解决所有问题

````



### 11：禁止异常流出 destructors 之外

````C++
// 见 Effective C++ 9
````



### 12：了解“抛出一个 exception”与“传递一个参数”或“调用一个虚函数”之间的差异

````C++
/* 本小节说明了函数的参数和 catch 捕捉异常的参数的不同 */

/* 1、函数的调用控制权最终回到调用端，但是当你抛出一个异常，控制权不会再回到抛出端 */

/* 
* 2、被捕捉的异常无论是值传递还是引用传递，都会发生复制行为，catch 子句拿到的正式那个副本，
*    如果是对象的复制调用的拷贝构造函数，并且是相对于对象的静态类型来拷贝构造的(实行动态见25)
*/

class Widget{};
class SpecialWidget: public Widget{};
void func(){
    SpecialWidget localSpcialWidget;
    Widget& rw = localSpcialWidget;
    throw rw; // 抛出一个类型为 Widget 的对象
}

/* 下面有两种方式接受该异常 */ 

catch(Widget& w){
    //...
    throw; // 传播该异常(不再构造副本)
}

catch(Widget& w){
    //...
    throw w; // 传播被捕捉异常的副本
}

/* 3、捕捉异常不允许隐式转换，但是可以捕捉继承架构中的类转换(派生类的异常可以由父类接受) */

void func(){
    int value = 100;
    //...
    throw value;
}
    
catch(double b){ // 并不能捕捉
    //...
}


// 值得注意是异常的接受是按照顺序的，并不会最佳匹配
class Base{};
class Derived: public Base{};

void func(){
    Derived d;
    //...
    throw d;  // 尽管抛出的是派生类，但是可以用基类捕捉
}

catch(Base b){
    //...
}
catch(Derived d){ // 永远都不会执行
    
}

// 指针型传递见13

````



### 13：以 by reference 方式捕捉 exceptions

````C++
/* 直入正题，by point 捕捉 exceptions 的缺点 */

/* 
* 首先通过指针捕捉同样会发生复制行为，但是复制的仅仅是指针本身，而指向的东西并没有发生复制
* 这就引出了作用域问题，以及是否需要 delete 该指针，总之以指针来捕获异常是不明智的行为
*/




/* by value 捕捉 exceptions 的缺点 */

/*
* 通过值来捕捉异常不需要考虑内存释放的问题，但是它也有自己的问题，继承架构中捕捉基类异常，同样可以
* 捕捉派生类异常，但是如果是通过值来捕捉的话就会引起切割问题，即在 catch 内部将其视为基类，即使
* 是虚函数的调用也将解析为基类的虚函数。
*/

class exception{
public:
    virtual void error_message();
    //...
};

class running_error: public expection{
public:
    void error_message();
};

void func(){
    //...
    throw running_error();
}

{ // block
    catch(exception ex){ // by value
        ex.error_message(); // 调用的是 exception::error_message
    }
}




/* 但是上面的问题如果通过引用传递将全部解决 */

{ // block
    catch(exception& ex){ // by reference
        ex.error_message(); // 调用的是 running_error::error_message
    }
}

````



### 14：明智运用 exception specifications

````C++
/* 了解即可，C++ 11 已经将其摒弃 */
````



### 15：了解异常处理的成本

````C++
/* 略 */
````





## 四、效率



### 16：谨记 80-20 法则

````C++
/* 程序大多数的执行时间耗费在小部分代码上，这样有利于后续程序的优化 */
````



### 17：考虑使用 lazy evaluation

````C++
/* 真正用到的时候才执行相关操作，用到哪一部分取哪一部分，需要哪一部分计算哪一部分 */
````



### 18：分期摊还预算的成本

````C++
/* 同 lazy evluation 相反，它是提前完成某种任务 */
/* 略，一种思想，理解不难 */
````



### 19：了解临时对象的来源

````C++
/* 当你产生了一个 non-heap-object 并且没有为它命名，一个临时对象就产生了 */
/* 发生该种情形存在两种情况，一是隐式转换，二是函数返回对象 */

/* 1、当隐式转换 */

void countChar(const string& str, char ch); // 一个字符在字符串中出先得个数
char str[1024];
char ch;
/* 如此调用会发生隐式转换(char* -> string&),会产生临时对象，所以在函数 countChar 所操作的 str 并不是
原数据，而是临时对象，但是可以调用成功，因为这个函数保证是只读的(限定符 const) */
countChar(str, ch);
/* 反之如果没有限定符 const 是错误的，不保证只读，如果在函数中更改临时对象是没有意义的 */



/* 2、返回值 */

const Number operator+(const Number& n1, const Number& n2);
/* 
* 这种函数很明显是返回的临时对象，如果你想妄想返回引用，来免除临时对象的构造析构造成的成本，
* 那是不可取的(见 Effective C++ 条款21)，但是本书的 条款20 可以解决这种问题 
*/

````



### 20：协助完成“返回值优化（RVO）”

````C++
/* 本节介绍的是由函数返回所产成临时对象造成额外的成本之解决方法 */

class Rational{
public:
    //...
private:
    int numerator; // 分子
    int denominator; // 分母
};

// non-member funcation，返回值为 const-value，一个有效率的实现
const Rational operator*(const Rational& lhs,const Rational& rhs){
    return Rational(lhs.numerator*rhs.numerator,lhs.denominator*rhs.denominator);
}

Rational a(1,2);
Rational b(3,5);
// 如此调用编译器可能会消除 operator* 所产生的临时对象，将 return 表达式所定义的对象构造于 c 内
Rational c = a * b;

// 将其声明为 inline 更好
inline const Rational operator*(const Rational& lhs,const Rational& rhs){
    return Rational(lhs.numerator*rhs.numerator,lhs.denominator*rhs.denominator);
}

// 值得注意的是，现在，即使为 operator* 所返回的对象命名，编译器依然可以优化，像这样
inline const Rational operator*(const Rational& lhs,const Rational& rhs){
    Rational temp(lhs.numerator*rhs.numerator,lhs.denominator*rhs.denominator);
    return temp;
}

````



### 21：利用重载技术避免隐式类型转换

````C++
/* 本节很简单，但是需要注意一些问题，我们依然用 Rational 为例 */

class Rational{
public:
    Rational(int numerator = 0, int denominator = 1); // 可以说是一个单自变量的构造
    //...
private:
    int numerator; // 分子
    int denominator; // 分母
};

const Rational operator*(const Rational& lhs,const Rational& rhs){
    return Rational(lhs.numerator*rhs.numerator,lhs.denominator*rhs.denominator);
}

// 考虑如此调用
Rational a(1,2);
Rational b = 3 * a; // 调用 operator*(Rational(3),a),但是产生了隐式转换导致产生临时变量，造成额外成本


// 一种方法是重载 operator* , 在面对 Rational b = 3 * a; 能够有特定的处理方法
const Rational operator*(int lhs, const Rational& rhs);
const Rational operator*(const Rational& lhs, int rhs);


/* 值得注意的是下面这种重载 */
const Rational operator*(int lhs, int rhs); // 错误，每一个重载操作符至少获得一个用户自制的自变量
/* 这是防止"防止天下大乱"，考虑下面的代码 */

int num = 3 * 6;

````



### 22：考虑以操作符符合形式（op=）取代其独身形式（op）

````C++
/* C++ 并不考虑在 operator+ 和 operator+= 之间设立任何互动关系，因此你得自己实现 */

class Ratioal{
public:
    Rational& operrator+=(const Rational& rhs);
    //...
private:
    int numerator; // 分子
    int denominator; // 分母
}

Rational& Rational::operator+=(const Rational& rhs){
    //...
    return *this;
}

/* 以 operator+= 实现 operator+ 是可行的,反之则不然  */
const Rational& operator+(const Rational& lhs, const Rational& rhs){
    return Rational(lhs)+=rhs
}

````



### 23：考虑其他程序库

````C++
/* 如题，详细看书 */
````



### 24：了解 virtual functions、multiple inheritance、virtual base classes、runtime type identification 的 成本

````C++
/* 未完待续 */
````





## 五、技术



### 25：将 constructor 和 non-memberfunction虚化

````C++
/* 首先虚的构造函数是不存在的，这里的意思是一种函数，给予其不同的输入，产生不同的对象 */

// 下面代码示例

class Base{
public:
    virtual Base* clone() const = 0;
    //...
}

class DerivedA: public Base{
public:
    virtual DerivedA* clone() const{
        return new DerivedA(*this);
    }
}

class DerivedB: public Base{
public:
    virtual DerivedB* clone() const{
        return new DerivedB(*this);
    }
}

/* 总之是利用虚函数，non-member 同样 */

class Base{
public:
    virtual void func() = 0;
    //...
}

class DerivedA: public Base{
public:
    virtual void func();
    //...
}

class DerivedB: public Base{
public:
    virtual void func();
    //...
}

void funcation(Base* b){
    b.func(); // 利用虚函数动态调用
    //...
}

// 由此可见不是什么新技术

````



### 26：限制某个 class 所能产生的对象

````c++
/* 允许零个或一个对象 */

class Printer{ // 打印机
public:
	//...
friend Printer& thePrinter();
private:
    // 将构造函数设置成默认
    Printer();
    Printer(const Printer& rhs); // 防止生成默认拷贝构造
};

Printer& Printer::thePrinter(){
    static Printer P;
    return P;
}

// 当然不是这一种实现方式，可以把Printer::thePrinter 设置成 static,利用 namespace 更好
/*
* 注意的是无论是友元或静态，异或施加于 namespace,这个静态的 Printer 对象都是属于函数的，
* 因此只有函数被调用的时候才会被初始化，所以函数每次被调用时检查对象是否诞生
*/



/* 限制固定的个数 */

class TooManyException : public std::exception { // 如果对象个数超出某个设定范围，抛出该异常
public:
    TooManyException(const char* message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

class Test{
public:
    Test(){
        if(MAX_NUM == 0){
            throw TooManyException("Too Many");
        }
        --MAX_NUM;
    }

    ~Test(){
        ++MAX_NUM;
    }
private:
    static int MAX_NUM;  // 同时存在的最大个数
};

// class static 义务性定义，不仅需要在 class 之内声明，还需要要在 class 之外定义
int Test::MAX_NUM = 5;

/* 
* 对象可在三种状态下生存：1、它自己 ，2、派生物的 base 成分，3、内嵌于较大对象之中 
* 如果这也是你维护类的个数所需要追踪的目标，那么没问题，但是通常可能只是想单纯的追踪状态1而已
* 那上面的代码时显然不行的
*/


/* 不同对象的构造状态 */

/* 
* 一个事实是: 带有 private constructors 的 classes,不能当作 base classes,也不能被内嵌于其他对象,
* 因此如果使一个函数不能被派生和内嵌于其他类中的一个做法是将它的构造设置成 private,但是如何构造呢
* 可以使用一层间接函数
*/

class Test{
    static Test* makeTest(){
        return new Test();
    }
    static Test* makeTest(const Test& rhs){
        return new Test(rhs);
    }
private:
    Test();
    Test(const Test&);
};

// 但是你必须手动的 delete,当然你也也可以使用智能指针




/* 设计一个可以计数的基类 */

class TooManyException : public std::exception {  // 同上的异常
public:
    TooManyException(const char* message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};


template <class T>
class Counted{  // 计数基类
public:
    static int numObject(){return curNum;}   // 提供的一个外部接口
protected:
    Counted();
    Counted(const Counted& rhs);
    ~Counted(){-- curNum;}
private:
    static int curNum;
    static const int MAX_NUM;
    void init();
};

template <class T>
Counted<T>::Counted(){init();}

template <class T>
Counted<T>::Counted(const Counted &rhs){init();}

template <class T>
void Counted<T>::init(){
    if(curNum >= MAX_NUM){throw TooManyException("Too Many");}
    ++curNum;
}

template <class T>
int Counted<T>::curNum = 0;  // 义务性的外部定义，应该同基类在同一个文件


class Test: private Counted<Test>{
public:
    using Counted<Test>::numObject; // 因为是 private 继承因此必须将基类的这个函数在派生类暴露出来 
    Test(){}  // 将 Test 类设置成 public，因此它可以在三种状态下存活(见上)，并且都会被计数
    ~Test(){}
};

template<> // 特例化
const int Counted<Test>::MAX_NUM = 5;  // 用户必须自己声明

````



### 27：要求或禁止对象产生于 heap

````C++
/* 要求对象产生于 heap 内 */

/*
* 直入正题：将析构函数声明为 private 就可以防止其产生于 heap 之外，而我们自己调用析构函数时可以使用
* 一层间接函数。不过这就限制了继承和内含(根据某物实现)，如果希望继承的话就将析构函数声明为 protected,
* 如果希望内含的话，就使用指针，示例我就略了
*/



/* 判断对象是否在 heap 之内 */

/*
* 没有完美的解法，移植性和健壮性二者不可兼得，无论是重写 operator new 为了标记某个类，并在构造的时候判断
* 这个标记，还是利用栈地址和堆地址的不同来设计都不能完美实现(我也不演示了)，
* 所以如果你的类需要判断是否在 heap 内，那你最好重新设计你的类
*/



/* 判断 delete 是否安全 */

/*
* 似乎也不是多么完美的方法，就是在 operator new 上做操，让每次经过 new 申请的内存地址都加入到
* 一个数组内，然后在 delete 的时候判断
*/


/* 禁止对象产生于 heap 内 */
/*
* 令 operator new 为私有，但是这样会对继承有影响，如果你想令其派生类申请于 heap，其代替方法，就是
* 令其成为一个成员变量
*/

````



###  28：smart pointer（智能指针）

````C++

/* 直入正题，实现一个简单的智能指针类 */
/* 值得注意的是对于智能指针类的赋值，这里采用管理转移的做法 */

template <class T>
class SmartPtr {
public:
    SmartPtr(T* rawPointer);
    SmartPtr(SmartPtr<T>& rhs);   // 注意这里的 rhs 是 non-const
    ~SmartPtr();

    T* operator->();
    T& operator*();
    const SmartPtr<T>& operator=(SmartPtr<T>& rhs); // 注意这里的 rhs 是 non-const

private:
    T* Pointee; // 原始指针
    T* release(); // 释放对当前指针的管理，并将其返回
};


template <class T>
SmartPtr<T>::SmartPtr(T* rawPointer)
    : Pointee(rawPointer)
{}

template <class T>
SmartPtr<T>::SmartPtr(SmartPtr<T>& rhs)
    : Pointee(rhs.release())  // rhs 已经改变了，所以是 non-const
{}

template <class T>
SmartPtr<T>::~SmartPtr()
{
    if (Pointee != nullptr) {
        delete Pointee;
    }
}

template <class T>
T* SmartPtr<T>::operator->()
{
    return Pointee;
}

template <class T>
T& SmartPtr<T>::operator*()
{
    return *Pointee;
}

template <class T>
const SmartPtr<T>& SmartPtr<T>::operator=(SmartPtr<T>& rhs)
{
    // 首先不会出现自我赋值的情况
    if (Pointee != nullptr) { delete Pointee; }
    Pointee = rhs.release(); // rhs 已经改变了，所以是 non-const
    return *this;
}

template <class T>
T* SmartPtr<T>::release()
{
    T* temp = Pointee;
    Pointee = nullptr；
    return temp;
}


class Test {  // 测试类
public:
    Test() { std::cout << "Test Constructing\n"; }
    ~Test() { std::cout << "Test Destructing\n"; }
};

int main(){
    Test* t1 = new Test;
    
    /* 这个是行为是我们期望它优化为 SmartPrt sp1(t1),但是编译器可不一定，也许它可以把 t1 隐式转换成
    SmartPtr<Test>,然后调用 SmartPtr<T>::SmartPtr(SmartPtr<T>& rhs) 拷贝构造函数，正常情况下
    没有什么问题，但是很不幸这个不是正常情况，由于策略原因 rhs 是 non-const(上面已解释)，C++ 明确表明
    临时对象是不能绑定于 non-const 对象上的，但是又不能将其置为 const,所以编译器如果按照第二种策略
    优化的话就编译错误了，所以为什么不直接写成 SmartPtr<Test> sp1(t1);*/
    SmartPrt<Test> sp1 = t1; // 假设按照期望运行
    SmartPtr<Test> sp2(new Test);
    
    /* 这个是编译必错的，原因还是临时对象是不能绑定于 non-const 对象上的(t2 产生了临时对象) */
    Test* t2 = new Test;
    sp2 = t2; 
}


/* 如何能让智能指针类模仿类继承体系中的基类指针向派生类的转换(按照书上的没有实行出来，待我修炼归来) */
/* 书上是 C++98,所以 auto_ptr早移除了，但是学习其中的思想也是极好的 */

````



### 29：引用计数（reference counting）

````C++
/*
* 使用的思想就是条款17：考虑使用 lazy evaluation，当某个对象被复制的时候，并没有真正的被复制，
* 而是让引用计数自增，当以只读的形式访问时我们什么都不需要做，但是存在读的时候我们就为这个对象申请一个
* 新的内存，防止它影响其他的对象。因此这个方式是否需要使用，你需要评估你的程序，是否存在大量的对象复制，
* 并且这些对象被写的可能性不大，这需要你自己斟酌，否则使用这种技术可能不仅不会提升你程序的效率，
* 反而会降低。
*/


/* 实现一个使用引用计数 String类 */

#include <iostream>
#include <cstring> 

class String{
public:
    String(const char* initValue = "");
    String(const String& rhs);

    String& operator=(const String& rhs);
    ~String();

  // 当存在写的时候会调用 non-const 版本(但是现在我们无法分辨，所以编译器不会调用 const版本，欲辨见 条款30)
    char& operator[](int index);    
    const char& operator[](int index) const;  // const 版本只读的时候调用

    const char* getValue(){   // 测试用
        return value->data;
    }

private:
    struct StringValue{   // 储存实际数据的类(虽然是结构体)
        int refCount; // 引用计数
        char* data;   // data

        StringValue(const char* initValue);
        ~StringValue();
    };

    StringValue* value; // StringValue类型指针，value->data 是字符串数据所在
};

String::String(const char* initValue)   // String 的构造函数会调用 StringValue 的构造
    : value(new StringValue(initValue))
{}

/* 拷贝构造函数并没用产生新的字符串，而是共享了 value 指针，并引用计数加一，注意这个计数是在 value 内
value 是一个指针 */
String::String(const String& rhs)  
    : value(rhs.value)
{
    ++ value->refCount;
}

/* 重载了 operator= */
String& String::operator=(const String& rhs){
    if(value == rhs.value){return *this;} // 见 Effectiva 条款 10 & 11
    if(--value->refCount == 0){ // 没有 其他的 String 类使用该字符串，就将其释放
        delete value;
    }
    value = rhs.value;
    ++value->refCount; // 自增
    return *this;
}

// 所用的 String 调用 operator[] 时我们无法区分读写，所以我们都假设成写，因此为它 new 新的字符串
char& String::operator[](int index){
    if(value->refCount > 1){ // 如果当前字符串只有它在使用就无需 new
        --value->refCount;
        value = new StringValue(value->data); // 调用拷贝构造
    }
    return value->data[index];   // 注意返回的是引用
}

const char& String::operator[](int index) const{
    return value->data[index];
}

String::~String(){  // 析构的时候判断引用计数，以确定是否需要 delete value
    if(--value->refCount == 0){delete value;}
}

String::StringValue::StringValue(const char* initValue)
    : refCount(1)            // 构造的时候 引用计数被自动置为 1
{
    data = new char[strlen(initValue)+1];
    strcpy(data, initValue);
}

String::StringValue::~StringValue(){
    delete [] data;
}

{ // Test block
    String s1("Effective");
    std::cout<<s1[1]<<std::endl; // 调用 String::operator[],虽然是读的，但是我们无法区分
}





/* 上面旨在于实现一个特殊的类 String,但是我们应该希望把一些功能抽象出来，比如计数之类的 */

/* 首先先将 refCount 的操作抽线出来 */

class String{   // 新实现的 String 并没有什么不同，只是内含的 StringValue 继承自一个 RCObject 类
public:
    String(const char* initValue = "");
    String(const String& rhs);

    String& operator=(const String& rhs);
    ~String();

    char& operator[](int index);
    const char& operator[](int index) const;

private:
    struct StringValue: public RCObject{ // Object 类内用来实现对 refCount 的操作
        char* data;
        StringValue(const char* initValue);
        ~StringValue();
    };
    StringValue* value;
};


class RCObject{
public:
    RCObject();
    RCObject(const RCObject&);
    RCObject& operator=(const RCObject& rhs);
    virtual ~RCObject() = 0;

    void addReference();
    void removeReference();
    void markUnshareable();
    bool isShareable() const;
    bool isShared() const;  // 数据是否被共享，即 refCount 是否大于 1

private:
    int refCount;  // 原本在 StringValue 类中，现在在基类中
    
    /* 表示 该字符串是否可以共享，我们最开始将其初始化为 true,表示可以共享，当我们出现写数据操作的时候 
    将其置为 false，并不可改变，当该类作为其他 String 类的拷贝构造参或者拷贝赋值参数的时候会判断
    是否可共享，以确定是否 new，这样其实用到条款18的思想  */
    bool shareable; 
};


/* String 类实现 */
String::String(const char* initValue)
    : value(new StringValue(initValue))
{}

String::String(const String& rhs)
{
    if(rhs.value->isShareable()){  // 上已经解释
        value = rhs.value;
        value->addReference(); 
    }
    else{
        value = new StringValue(rhs.value->data);
    }
}

String& String::operator=(const String& rhs){
    if(value == rhs.value){return *this;}
    value->removeReference();
    if(rhs.value->isShareable()){ // rhs.value 是否可以共享
        value = rhs.value;
    }
    else{
        value = new StringValue(rhs.value->data);
    }
    value->addReference();
    return *this;
}

char& String::operator[](int index){
    if(value->isShared()){ // 是否被共享
        value->removeReference();
        value = new StringValue(value->data);
        value->addReference();
    }
    value->markUnshareable();  // 将 isShareable 置为 false
    return value->data[index];
}

const char& String::operator[](int index) const{
    return value->data[index];
}

String::~String(){
    value->removeReference();
}

String::StringValue::StringValue(const char* initValue)
{
    data = new char[strlen(initValue)+1];
    strcpy(data, initValue);
    addReference(); // 因为 RCObject 将 refCount 初始化为0，所以这里在得到字符串的时候需要自增
}

String::StringValue::~StringValue(){
    delete [] data;
}



/* RCObject 类实现 */
RCObject::RCObject()
    : refCount(0), shareable(true)
{}

RCObject::RCObject(const RCObject &)
    : refCount(0), shareable(true)
{}

RCObject& RCObject::operator=(const RCObject &rhs){
    return *this;
}

RCObject::~RCObject(){}

void RCObject::addReference()
{
    ++refCount;
}

void RCObject::removeReference()
{
    if(--refCount <= 0){delete this;}
}

void RCObject::markUnshareable()
{
    shareable = false;
}

bool RCObject::isShareable() const
{
    return shareable;
}

bool RCObject::isShared() const
{
    return refCount > 1;
}




/* 上述代码随然将计数功能抽离出来，但是调用还是需 String 和 StringValue,接下来我们就解决这个问题 */


class String
{
public:
	String(const char* initValue = "");
	const char& operator[](int index) const;
	char& operator[](int index);
    
private:
	struct StringValue : public RCObject
	{
		char* data;
 		
		StringValue(const char* initValue);
		StringValue(const StringValue& rhs);
		void init(const char* initValue);
		~StringValue();
	};
	RCPtr<StringValue> value;  // 增加了一个引用计数指针类
};

class RCObject
{
public:
	void addReference();
	void removeReference();
	void markUnshareable();
	bool isShareable();
	bool isShared();

protected:
	RCObject();
	RCObject(const RCObject& rhs);
	RCObject& operator=(const RCObject& rhs);
	virtual ~RCObject();

private:
	int refCount;
	bool shareable;
};



template <class T>
class RCPtr   // 引用计数指针类
{
public:
	RCPtr(T* realPtr = 0);
	RCPtr(const RCPtr& rhs);
	~RCPtr();
	RCPtr& operator=(const RCPtr& rhs);
	T* operator->() const;
	T& operator*() const;

private:
	T* pointee;
	void init();
};

template<class T>
inline void RCPtr<T>::init()   // 由下面的实现所提取的公共部分
{
	if (pointee == 0) { return; }
	if (pointee->isShareable() == false) {
		pointee = new T(*pointee);
	}
	pointee->addReference();
}

template<class T>
inline RCPtr<T>::RCPtr(T* realPtr)
	: pointee(realPtr)
{
	init();
}

template<class T>
inline RCPtr<T>::RCPtr(const RCPtr& rhs)
	: pointee(rhs.pointee)
{
	init();
}

template<class T>
inline RCPtr<T>::~RCPtr()
{
	if (pointee) {
		pointee->removeReference();
	}
}

template<class T>
inline RCPtr<T>& RCPtr<T>::operator=(const RCPtr& rhs)
{
	if (pointee != rhs.pointee) {
		if (pointee) { pointee->removeReference(); }
		pointee = rhs.pointee;
		init();
	}
	return *this;
}

template<class T>
inline T* RCPtr<T>::operator->() const
{
	return pointee;
}

template<class T>
inline T& RCPtr<T>::operator*() const
{
	return *pointee;
}


/* String 的实现 */
String::String(const char* initValue)
	: value(new StringValue(initValue))
{}

const char& String::operator[](int index) const
{
    // 别忘了 value 是一个 RCPtr<StringValue> 类型，编译器实际上：(value.operator->())->data[index];
	return value->data[index];  
}

char& String::operator[](int index)
{
	if (value->isShared()) {
/*下面的 value = new StringValue(value->data); 会发生什么？虽然是短短的一句代码，但是仔细分析
下来还是挺复杂的，我认为这行代码贯穿了这几个类。
1、执行 value->data,value是一个RCPtr<StringValue> 类型，所以实际的调用(value.operator->())->data;
	它返回的是一个 char*
2、执行 new StringValue(char*),即 StringValue 的拷贝构造，返回一个 StringValue*
3、执行 value=StringValue*; 发生隐式转换，所以实际上调用 value=RCPtr<StringValue>(StringValue*);
	因此这会产生临时变量，我们将其设定为 RCPtr<StringValue> temp,记住它会在此句结束后析构
4、执行 value=temp;调用 RCPtr<StringValue> 的 operator=(),即 value.operator=(temp);
	这句执行后，value 中的 refCount 是几？答案是 2，因为还有一个临时变量的存在，稍后它会自动析构
5、临时对象的析构，temp 将会自动析构，在这其中会自减 refCount，以达到我们的预期值 1。
这其中的函数调用，以及 refCount 的改变时机等等，感觉挺有意思的
*/
		value = new StringValue(value->data);
	}
	value->markUnshareable(); // 同上次实现的意义一样
	return value->data[index];
}

String::StringValue::StringValue(const char* initValue)
{
	init(initValue);
}

String::StringValue::StringValue(const StringValue& rhs)
{
	init(rhs.data);
}

void String::StringValue::init(const char* initValue)
{
	data = new char[strlen(initValue) + 1];
	strcpy(data, initValue);
}

String::StringValue::~StringValue()
{
	delete[] data;
}


/* RCObject 的实现 , 没有什么变化 */
RCObject::RCObject()
    : refCount(0), shareable(true)
{}

RCObject::RCObject(const RCObject& rhs)
    : refCount(0), shareable(true)
{}

RCObject& RCObject::operator=(const RCObject& rhs)
{
    return *this;
}

RCObject::~RCObject(){}

void RCObject::addReference() 
{
    ++refCount;
}

void RCObject::removeReference()
{
    if (--refCount == 0) {
        delete this;
    }
}

void RCObject::markUnshareable()
{
    shareable = false;
}

bool RCObject::isShareable()
{
    return shareable;
}

bool RCObject::isShared()
{
    return refCount > 1;
}




/* 继续抽离，将计数类施加到计数智能指针，得到 RCObject 和 RCSmartPtr */
class RCObject  // 它的实现就不重复了，与前面没有什么变化
{
public:
	void addReference();
	void removeReference();
	void markUnshareable();
	bool isShareable();
	bool isShared();

protected:
	RCObject();
	RCObject(const RCObject& rhs);
	RCObject& operator=(const RCObject& rhs);
	virtual ~RCObject();

private:
	int refCount;
	bool shareable;
};



template <class T>
class RCSmartPtr{
public:
    RCSmartPtr(T* realPtr = 0);
	RCSmartPtr(const RCSmartPtr& rhs);
	~RCSmartPtr();
	RCSmartPtr& operator=(const RCSmartPtr& rhs);
    const T* operator->() const; // 针对于只读
	T* operator->(); // 针对于写
    const T& operator*() const; // 针对于只读
	T& operator*(); // 针对于写

private:
    struct CountHolder : public RCObject{  // 使用 CountHolder 继承 RCObject
        ~CountHolder(){delete pointee;}
        T* pointee; // 原始指针
    };
    CountHolder* counter;
    void init();
    void makeCopy();  // 如果调用写操作就执行 makeCopy() 来 new 一个新的
};

template <class T>
inline void RCSmartPtr<T>::init()
{
    if(counter->isShareable() == false){
        T* oldValue = counter->pointee;
        counter = new CountHolder;
        counter->pointee = new T(*oldValue);
    }
    counter->addReference();
}


template <class T>
inline RCSmartPtr<T>::RCSmartPtr(T *realPtr)
    : counter(new CountHolder)
{
    counter->pointee = realPtr;
    init();
}

template <class T>
inline RCSmartPtr<T>::RCSmartPtr(const RCSmartPtr &rhs)
    : counter(rhs.counter)
{
    init();
}

template <class T>
inline RCSmartPtr<T>::~RCSmartPtr()
{
    counter->removeReference();
}

template <class T>
inline RCSmartPtr<T> &RCSmartPtr<T>::operator=(const RCSmartPtr<T> &rhs)
{
    if(counter != rhs.counter){
        counter->removeReference;
        counter = rhs.counter;
        init();
    }
    return *this;
}


template <class T>
inline void RCSmartPtr<T>::makeCopy()
{
    if(counter->isShared()){
        counter->removeReference();
        T* oldValue = counter->pointee;
        counter = new CountHolder;
        counter->pointee = new T(*oldValue);
        counter->addReference();
    }
}


template <class T>
inline const T *RCSmartPtr<T>::operator->() const
{
    return counter->pointee;
}

template <class T>
inline T *RCSmartPtr<T>::operator->()
{
    makeCopy();
    return counter->pointee;
}

template <class T>
inline const T &RCSmartPtr<T>::operator*() const
{
    return *(counter->pointee);
}

template <class T>
inline T &RCSmartPtr<T>::operator*()
{
    makeCopy();
    return *(counter->pointee);
}

````



### 30：Proxy classes（替身类、代理类）

````C++
/* 在29的 String 类中我们无法确定 operator[] 的调用目的是读还是写，这一节将会有答案 */

class String{
public:
    class CharProxy{
  	public:
        CharProxy(String& str, int index);
        CharProxy& operator=(const CharProxy& rhs); // 左值运用，被用来写
        CharProxy& operator=(char c);
        operator char() const; // 右值运用，被用来读
    private:
        String& theString; // 这个代理字符所代理的字符串
        int charIndex;     // 相应的索引
    };
    const CharProxy operator[](int index) const;// 针对 const String
    CharProxy operator[](int index); // 针对 non-const String
    friend class CharProxy;          

private:
    RCPtr<StringValue> value;
};

const String::CharProxy String::operator[](int index) const
{
    return CharProxy(const_cast<String&>(*this), index);
}

String::CharProxy String::operator[](int index)
{
    return CharProxy(*this, index);
}


String::CharProxy::operator char() const
{
    return theString.value->data[charIndex];
}

String::CharProxy& String::CharProxy::operator=(const CharProxy &rhs)
{
    if(theString.value->isShared()){
        theString.value = new StringValue(theString.value->data);
    }
    theString.value->data[charIndex] = rhs.theString.value->data[rhs.charIndex];
    return *this;
}

String::CharProxy& String::CharProxy::operator=(char c)
{
    if(theString.value->isShared()){
        theString.value = new StringValue(theString.value->data);
    }
    theString.value->data[charIndex] = c;
    return *this;
}


/* 当然代理类亦有缺点，就是它很难真正的完全模仿实际的对象 */

void swap(char& a,char& b);
String str = "+C+";
swap(str[0],str[1]); // 错误,不能接受 String::CharProxy 的对象，它可以隐式转换成 char不是char&

/* 假设我们有一个储存 Array 类，同样采用引用计数的技术，其中有个代理类来区分读写 */

class Array{
public:
    class Proxy{
        Proxy(Array<T>& array, int index);
        Proxy& operator=(const T& rhs);
        operator T() const;
    };
    const Proxy operator[](const T& rhs) const;
    Proxy operator[](const T& rhs);
    //...
};


Array<int> intArr;
intArr[5] = 22;  // 正确
intArr[1]++;     // 错误  Array<int>::Proxy::operator++() 没有这样一个函数
intArr[12]+=14;  // 同上

// 如果需要模仿真正的类也许需要写许多函数，比如 Array<Rational>,Rational 类有许多 member 函数。

````



### 31：让函数根据一个以上的对象类型来决定如何虚化

````C++
/* 略 */
````





## 六、杂项讨论



### 32：在未来时态下发展程序

````C++
/* 略 */
````





### 33：将非尾端类（non-leaf classes）设计成抽象类（abstract）

````C++
/* 看下面代码 */

/* 目的是实现在继承体系下的同类型直接的赋值 */

class Base{
public:
    Base(int baseValue):_baseValue(baseValue){}
    ~Base(){}
    Base& operator=(const Base& rhs){
        _baseValue = rhs._baseValue;
        return *this; 
    }
    virtual void showAll(){std::cout<<"Base "<<_baseValue<<std::endl;}
protected:
    int _baseValue;
};


class DerivedA: public Base{
public:
    DerivedA(int baseValue,int derivedAValue)
        : Base(baseValue),_derivedAValue(derivedAValue)
    {}
    ~DerivedA(){}
    DerivedA& operator=(const DerivedA& rhs){
        Base::operator=(rhs);
        _derivedAValue = rhs._derivedAValue;
        return *this;
    }

    virtual void showAll(){std::cout<<"DerivedA "<<_baseValue<<" "<<_derivedAValue<<std::endl;}
private:
    int _derivedAValue;
};


class DerivedB: public Base{   // 实现同 DerivedA 一模一样，但是忽视这个，它们是不同的类
public:
    DerivedB(int baseValue,int derivedBValue)
        : Base(baseValue),_derivedBValue(derivedBValue)
    {}
    ~DerivedB(){}
    DerivedB& operator=(const DerivedB& rhs){
        Base::operator=(rhs);
        _derivedBValue = rhs._derivedBValue;
        return *this;
    }
    virtual void showAll(){std::cout<<"DerivedB "<<_baseValue<<" "<<_derivedBValue<<std::endl;}
private:
    int _derivedBValue;
};


int main(){
    Base* a1 = new DerivedA(12,13);
    Base* a2 = new DerivedA(10086,9898);
    *a1 = *a2;    //没有任何编译问题但是，调用的 a1.Base::operator=(a2),产生了部分赋值的现象
    a1->showAll();
}


/* 显然这样的问题并不是我们所希望的，为了解决这个问题，也许可以把赋值运算符写成虚的 */

/* 类似这样吧,重写时参数列表必须相同 */
virtual Base& Base::operator=(const Base& rhs);
virtual DerivedA& DerivedA::operator=(const Base& rhs);
virtual DerivedB& DerivedB::operator=(const Base& rhs){
    Base::operator=(rhs);
    _derivedBValue = rhs._derivedBValue;  // 编译错误，没有 Base::_derivedBValue
    return *this;
}

/* 退一步来说，假如在赋值时并没有上面的问题，我们必须允许下面的赋值 */
Base* a1 = new DerivedA(12,13);
Base* a2 = new DerivedB(10086,9898);
*a1 = *a2; // 动态类型不同的类之间赋值，这与我们先前的目的背道而驰


/* 
* 既然上面的都不行，我们能不能在出现局部赋值和异型赋值出行的时候不能通过编译
* 令 Base成为一个抽象类即可，但是 Base是存在数据的，而抽象类是不允许实例化的，因此我们可以提取Base
* 成为 AbstractBase,在令Derived和Base都继承于它
*/

class AbstractBase{
public:
    AbstractBase(int baseValue):_baseValue(baseValue){}
    ~AbstractBase(){}
    virtual void showAll() const = 0; 
protected:
    int _baseValue;
    AbstractBase& operator=(const AbstractBase& rhs){
        _baseValue = rhs._baseValue;
    }
};


class Base: public AbstractBase{
public:
    Base(int baseValue):AbstractBase(baseValue){}
    ~Base(){}
    Base& operator=(const Base& rhs){}
    virtual void showAll() const{std::cout<<"Base "<<_baseValue<<std::endl;}
};


class DerivedA: public AbstractBase{
public:
    DerivedA(int baseValue,int derivedAValue)
        : AbstractBase(baseValue),_derivedAValue(derivedAValue)
    {}
    ~DerivedA(){}
    DerivedA& operator=(const DerivedA& rhs){
        AbstractBase::operator=(rhs);
        _derivedAValue = rhs._derivedAValue;
        return *this;
    }

    virtual void showAll() const{std::cout<<"DerivedA "<<_baseValue<<" "<<_derivedAValue<<std::endl;}
private:
    int _derivedAValue;
};


class DerivedB: public AbstractBase{
public:
    DerivedB(int baseValue,int derivedBValue)
        : AbstractBase(baseValue),_derivedBValue(derivedBValue)
    {}
    ~DerivedB(){}
    virtual DerivedB& operator=(const DerivedB& rhs){
        AbstractBase::operator=(rhs);
        _derivedBValue = rhs._derivedBValue;
        return *this;
    }
    virtual void showAll() const{std::cout<<"DerivedB "<<_baseValue<<" "<<_derivedBValue<<std::endl;}
private:
    int _derivedBValue;
};


int main(){
    AbstractBase* a1 = new DerivedA(12,13);
    AbstractBase* a2 = new DerivedA(10086,9898);
    *a1 = *a2;   // 不能通过编译
    a1->showAll();
}

/* 
* 既然解决这个问题很难，那么就让这个问题不存在。这很合理，因为这行代码不能通过编译，只有同类赋值才能，
* 如果通过编译就符合用户预期
*/

/* 这一节后面还有一部分，说实话看不太懂，另一方面这本书较老了 */

````



### 34：如何在同一个程序中结合 C++ 和 C

````C++
/* 略 */
````



### 35：让自己习惯于标准 C++

````C++
/* 略 */
````



### 



