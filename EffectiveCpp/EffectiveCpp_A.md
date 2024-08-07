# Effective C++ 上





## 一、让自己习惯 C++



### 01：视 C++ 为一个语言联邦



```c++
/* C++ 高效编程视情况而变化，取决于你使用的Cpp 的哪一部分 */

/*视C++为一个语言联邦而非一种单一语言，即C++有四个次语言组成*/

// C: 区块、语句、预处理器、内置数据类型、数组、指针。
// Object-Oriented C++: classes(构造&析构)、封装、继承、多态、virtual函数(动态绑定)
// Template C++: 泛式编程
// STL: 对容器、迭代器、算法以及函数对象有极佳的配合



```



### 02：尽量以 const, enum, inline 替换 #define

```c++
#define DOUBLE_NUM 3.14 // 因为预处理器的盲目替换可能导致目标码出现多份3.14
const double DoubleNum = 3.14; // 替换方式


#define AUTHOR "Tom"
const char* const author="Tom"; // 替换方式
const std::string author("Tom");



/* define 无法为class提供专属常量 */
/*通常C++ 要求你对你所使用的任何一个东西提供一个定义式*/
static const int num = 100; // 定义式
/* 如果在类中它是类的专属常量且又是static 且为整数类型(int||bool||char)可能会有例外*/
class MyClass{
private:
    static const int num = 100; // 声明式
    int arr[num]; // 可以编译通过
};
/* 事实是你只要不对它取地址，你就可以在它没有声明式的情况下使用它，否则你应该提供一个定义式 */
const int MyClass::num; // 已经在声明中获取到初始值，不需也无法在定义中重新提供


/* 如果不想让别人取地址可以使用enum */
class MyClass{
private:
    enum {num = 5};
    int arr[num];=
};



/* 
* define 还可以定义一些简单的函数比如Max之类的，利用预处理防止调用函数而带来的额外开销但是其问题也不少，
* 使用inline 代替是一个明智方法
*/
    
#define MAX(a,b) ((a)>(b) ? (a) : (b))
template<typename T>
inline T Max(const T& a, const T& b){
    return a>b ? a :b;
}
// 值得注意的是尽管声明了inline ,但是这只是对编译器的申请，并不一定按照内联的方式执行

```



### 03：尽可能使用const

````c++
/*
* const 如果出现在星号的左边表示被指物是常量，如果出现在右面表示指针是常量
*/


/* STL 迭代器是根据指针为模型实现的，即T* */
std::vector<int> vec;
...
const std::vector<int>::iterator iter = vec.begin(); // 其等同于 T* const
*iter = 10; // 正确
++iter; // 错误


std::vector<int>::const_iterator citer =  vec.begin(); // 其等同于 const T*
*iter = 10; // 错误
++iter; // 正确
    


/* const 成员函数 */
/*
*保护对象的状态： 当您声明一个成员函数为 const 时，编译器会强制执行保护对象的状态，防止在该函数内部无意间
*修改成员变量。这有助于确保对象的数据不会被意外更改。
*
*允许常量对象调用： 如果一个对象被声明为 const，则只能调用其 const 成员函数。这允许您在常量对象上执行只读
*操作，例如获取对象的属性或执行不会修改对象状态的计算。
*
*优化机会： 编译器可以使用 const 声明来进行优化。它可以安全地假定 const 函数不会修改对象的状态，从而在编
*译时进行一些优化，提高性能。
*/

/* 两个成员函数如果只有常量性不同，可以被重载 */
class Text{
private:
    std::string text;
public:
    /* 返回类型是const 是必须的，否则回引起编译错误，并且假如不是就违背了const 成员函数的本意，
    即返回的是char& 也就可以间接的更改类成员变量 */
    const char& operator[](std::size_t idx) const{
        return this->text[idx];
    }
    
    char& operator[](std::size_t idx){
        return this->text[idx];
    }

    Text(std::string text){this->text = text;}
};


int main(){
    Text t1("0xfff"); // t1[0] 调用的是 non-const 版本
    const Text t2("12345"); // t2[0] 调用的是 const 版本
    return 0;
}



/* bitwise constness(位常量性) and logical constness(逻辑常量性) */
/*
* 一个清晰的分辨就是一个类中所含的指针的变化，如果指针没有变化，这是符合位常量性和逻辑常量性的
* 但是如果该指针所指物发生变化这仍然符合位常量性,却不符合逻辑常量性，遗憾的是编译器很难对逻辑常量性
* 做出判断
*/
class CText{
private:
    char* text;
public:
    /* 注意此时char& 之前已经没有const(当然这是不合理的) 但是却可以编译通过，因为满足位常量性 */
    char& operator[](std::size_t idx) const{
        return this->text[idx];
    }
    
    char& operator[](std::size_t idx){
        return this->text[idx];
    }

    CText(std::string text){
        this->text = new char[100];
        std::strcpy(this->text,text.data());
    }
};

int main(){
    CText t1("0xfff");
    const CText t2("12345");
    t1[0] = 'M';
    t2[0] = 'M'; // 并没有报错，且已经实际改动了t2.text 值
    std::cout<<t1[0]<<"  "<<t2[0]<<std::endl;
    return 0;
}




/* 释放掉类成员变量中 non-static 的bitwise constness 约束 */
class PText{
private:
    ...
    mutable std::size_t textLength; //这个成员变量总是可变，即使是在const
    ...
}



/* 避免non-const 和const 成员函数中避免重复 */
class Text{
private:
    std::string text;
public:
    const char& operator[](std::size_t idx) const{
        // 重复的事务
        return this->text[idx];
    }
    
    char& operator[](std::size_t idx){
        // 重复的事务
        return this->text[idx];
    }
};
/*
* 一种可行的做法是其中的一个调用另一个，由于const 成员函数承诺绝不改变对象的逻辑状态
* 所以安全的做法是用const 调用non-const,还需要利用转型实现该操作
*/
class Text{
...
public:
    const char& operator[](std::size_t idx) const{
        // 实现的事务
        return this->text[idx];
    }
    
    char& operator[](std::size_t idx){
        return 
            const_cast<char&>( // 将const 转除
            static_cast<const Text&>(*this)[idx];// 转换成const 类型为了使其调用const版本
        );
    }
};

````



### 04：确定对象被被使用前已先被初始化

```c++
/* 成员初始化列表 */
class MyClass{
private:
    std::string name;
    std::gender day;
    int age;
    
public:
    /* 一种可能的构造函数 */
    MyClass(const std::string& name, const std::string& gender, int age){
        this->name = name;   // 这些都是赋值，而非初始化
        this->gender = gender;
        this->age = age;
    }
};
//它们的初始化发生在进入构造函数之前，但是内置类型int不是，一种高效的做法是使用成员初始化列表
MyClass(const std::string& name, const std::string& gender, int age)
    :name(name),
    gender(gender);
    age(age);
{}
//  对于初始化顺序是base 早于derived, 并且按照class 内的声明顺序，即使同初始列表顺序不同



/*
* 这一点是重要的
* 不同编译单元内的non-local static 对象的初始化顺序是不确定的
*/

/*======================= 源文件 A ======================*/

class A{
public:
    ...
    int funcA();
private:
    ...
};

A a; // non-local static

/*======================================================*/

/*======================= 源文件 B ======================*/

extern A a; // 预备使用的对象位于源文件A中

class B{
public:
    ...
    B(){
        a.funcA(); // 使用到a
    }
    ...
private:
    ...
};

B b; // 如果a在b之前没有初始化，将错误

/*======================================================*/

/*
* 解决方案很简单从这句话下手: 不同编译单元内的non-local static 对象的初始化顺序是不确定的
* 位于相同编译单元或者令它为local static 对象
*/
A& get_a(){
    static A a; // local static 对象
    return a
}

B(){
	get_a().funcA(); // 这样就能保证其初始化顺序
}

```





## 二、构造/析构/赋值运算



### 05：了解 C++ 默默编写并调用哪些函数

```C++
/* 
* 如果你没有为你编写的类提供构造、析构、拷贝、赋值函数，那么当你在程序中使用到的时候，编译器
* 回自动地为你创建这些函数的默认，这些函数是 public 和inline 的
* 如果你声明了，编译器则不会为你生成默认版本，哪怕是重载
*/

/*
* 所谓的默认拷贝构造和拷贝赋值，只是编译器会目标类的变量来拷贝初始化目标类的变量的bit
* 来完成初始化，但是如果出现问题，编译器会拒绝生成默认版本，下面是一个例子
*/

class Person{
private:
    std::string& name;
    const int age;
    
public:
    Person(std::string& name, const int age):
    name(name),age(age)
    {}
};

int main{
    Person p1("num80",98);
    Person p2("double3.14",97);
    p1 = p2; // 编译器会引发错误，
}


/*
* 另一种值得注意的情况是如果基类将拷贝赋值声明为private，编译器将无法为派生类生成拷贝赋值函数
* 因为编译器无法处理基类部分的拷贝，它无法调用派生类不能调用的private
*/

```



### 06：若不想使用编译器自动生成的函数，就该明确拒绝

```c++
/*
* 假如有一个关于地产的类，每一个地产都应该是独一无二的，但是如果你不在该类中声明拷贝构造和赋值构造，
* 你就默认同意了编译器应该在使用到的时候生成一个默认版本
*/

class HomeForSale{
    ...
    // 没有声明拷贝赋值和拷贝构造
};

int main(){
    HomeForSale h1;
    HomeForSale h2;
    HomeForSale h3(h1); // 就实际而言不应该通过编译
    h2 = h1;            // 同上
}


/*
* 一种简单的做法就是提供声明，但是不提供定义，且将其声明为private
*/

class HomeForSale{
public:
    ...
private:
    HomeForSale(const HomeForSale&);
    HomeForSale& operator=(const HomeForSale&);
};
// 可以预知的是如果在其成员或者友元函数中仍可以调用，并且调用之后会出现连接错误(你没有具体实现)


// 这种方法可以将错误从连接期提前到编译期
class uncopyable{
protected:// 允许派生类调用构造和析构
    uncopyable(){}
    ~uncopyable(){}
private: // 编译器将拷贝函数声明为private ，因此派生类无法自动生成拷贝函数，因为无法调用基类的拷贝
    uncopyable(const uncopyable&);
    uncopyable& operator=(const uncopyable&);
};

class HomeForSale:private uncopyable{
    ...
};
/*
* 值得注意的是HomeForSale是私有继承于基类uncopyable,这是合理的，反之若是公有继承则不合理，
* 因为共有继承是is-a 关系，否则就逻辑而言是不合理的，另一个理由则是空基优化(见39)
*/

```



### 07：为多态的基类声明virtual析构函数

```C++
/* 将基类的析构函数声明为virtual，至少要在有派生类公有继承它的时候 */

class TimeKeeper{ // 计时的类
public:
    TimeKeeper();
    ~TimeKeeper(); // 析构函数是 non-virtual
private:
    ...
};

/* 这些派生类中内部都有独特的实现 */
class AClock: public TimeKeeper{}; // A类钟
class ABClock: public TimeKeeper{}; // B类钟

/* 这是一个factory 函数 */
TimeKeeper* getTimeKeeper(int flag); // 根据参数的不同可以返回不同的钟指针，注意返回类型是基类指针

int main(){
    TimeKeeper* A_Clock = getTimeKeeper(0);// 返回一个A类的钟却由基类指针接收
    // doing
    delete A_Clocl; // 调用析构函数
    return 0;
}
/* 
* 上面出现的问题是一个由基类所指的派生类调用了析构函数，其关键是基类的析构函数是 non-virtual
* 这可能引发内存泄漏，因为C++ 明确指出这种情况通常发生的是派生类对象部分没有被析构，这就会
* 出现局部析构的奇怪现象。
* 解决方法就是将派生类的析构函数声明为 virtual.
*/
virtual ~TimeKeeper();


/*
* 作为一个基类其析构通常是virtual ，一个类中存在virtual 函数几乎也可以确定发其析构函数也是virtual， 
* 但是并不是记住将所有的类的析构函数都声明为virtual 就完事大吉，因为将其声明为 virtual,是需要付出
* 代价的，并不是简短的几个字母，还有需要考虑的内存，时间
*/


/*
* 将STL 库中的标准类作为基类继承或者说继承任何其析构函数是non-virtual 需要你的斟酌
* (STL 中string、vector、set等等其析构都是 non-virtual)
*/


/* pure virtual 函数导致抽线类 */
class abstractClass{
public:
    virtual ~abstractClass() = 0; //pure virtual 析构函数
}
// 尽管它是pure virtual，你依然需要给他一个定义，因为其派生类的构造需要它的参与，否则会出现连接错误
~abstractClass(){}; 

```



### 08：别让异常逃离析构函数

```c++
/* C++ 并不禁止析构抛出异常，但是析构函数抛出异常可能会引起程序过早结束或出现不明确行为 */

class DBConnection{ // 一个数据库连接的类
public:
    static DBConnection create(); // 返回一个数据库连接
    void close(); // 关闭连接，失败则抛出异常
};

class DBConn{ // 管理DBConnection 的类
public:
    ~DBConn(){
        db.close();
    }
private:
    DBConnection db;
};

/* 可能存在这样的代码 */
{ // block
    DBConn dbc(DBConnection::create()); 
} // dbc 会随着block 的结束自动析构，并且在析构中调用close，以防止用户遗忘

/* 如果close() 出现异常是糟糕的，为了不让该异常从析构函数中抛出，有两种解决方法 */
// 遇到异常直接终止程序
DBConn::~DBConn(){
    try{db.close();}
    catch{
        // 日志记录该异常
        std::about(); // 终止程序
    }
}

// 遇到异常忽略异常
DBConn::~DBConn(){
    try{db.close();}
    catch{
        // 日志记录该异常
        // 忽略异常
    }
}


/* 
* 显然这两种方法是没有办法的办法，一个较佳的策略是重写一个DBConn 的接口 
* 将close 的责任从DBConn 中的析构函数中分担出去
*/
class DBConn{ 
public:
    ~DBConn(){
        if(!closed){
            try{db.close();}
            catch{
                // 终止或忽略
            }
        }
    }
    
    void close(){
        db.close;
        closed = true;
    }
private:
    DBConnection db;
    bool closed;
};

{ // block
    DBConn dbc(DBConnection::create());
    dbc.close(); // 留个用户操作空间
}

```



### 09：绝不在构造和析构函数过程中调用 virtual 函数

```C++
/*
* 举个例子，下面是一个交易的的类，要有买进和卖出的订单，且交易要审计，所以每次创建对象时
* 要有个审计日志
*/

class Transaction{
public:
    Transaction(){
        ...
        logTranscation(); // 志记该交易
    }
    
    virtual void logTranscation() const = 0; // 因不同的类型做出不同的审计日志
}

class BuyTransacion: public Transaction{
public:
    virtual void logTranscation() const; // 省略其具体实现
    ...
}

class SellTransacion: public Transaction{
public:
    virtual void logTranscation() const; // 同上
    ...
}

int main(){
    // 创建一个BuyTransaction, 并期望其基类调用BuyTransaction 的logTranscation()
    BuyTransaction b;
    return 0;
}

/*
* 然而这个期望是不现实的，首先会出现连接错误，因为在调用基类的构造函数时会调用
* 基类的 logTranscation()的 pure virtual 版本，因为它没有相关定义所以连接错误
* 当然这并不是重点，因为就算你为它提供一个空定义，程序也不会按照你的期望运行。
* base class 构造期间virtual 函数绝不会下降到 derived class 阶层
*/

/* derived class 在调用derived 构造函数之前是 base class */
/* derived class 析构函数开始执行，对象内部的成员变量呈现未定义值，C++ 视它们不存在 */


/*
* 解决方法就是将 logTranscation()该为 non-virtual，并为父类的构造函数提供一些必要信息
*/

class Transaction{
public:
    Transaction(const std::string& logInfo){
        ...
        logTranscation(const std::string& logInfo); // 志记该交易
    }
    
    void logTranscation(const std::string& logInfo); // non-virtual
} 


class BuyTransaction: public Transaction{
public:
    BuyTransaction(const std::string& logInfo)
        : Transaction(logInfo){
            ...
        }
    
};

```



### 10：令 operator= 返回一个 reference to *this

```c++
/* 一个举例 */

int x,y,z;
x=y=z=10086;  // x=(y=(z=10086)

/* 
* 上面是一个连锁赋值的例子，逻辑上(z=10086) 不应该返回任何值，但是似乎现实中上连续赋值应该被满足
* 下面是个实现
*/

class INTER{
public:
    ...
    INTER& operator=(const INTER& rhs){
        ...
        return *this;
    }
    
    INTER& operator+=(const INTER& rhs){ // 同样使用于 *=、-=....
        ...
        return *this;
    }
};

/* 非特殊情况下，我们应该如此，而且内置类型和标准库类型都遵循此 */

```



### 11：在 operator= 中处理“自我赋值”

```C++
/* 自我赋值，顾名思义就是自己给自己赋值，听着很怪，其实无处不在 */
class widget{...};
widget w;
w = w; // 这样看着很怪，但是换成指针呢

arr[i]=arr[j]; // i == j
*pl = *pr;

/* 下面是自我赋值安全性带来的问题 */

class Value(){...};

class MyClass{
public:
    MyClass& operator=(const MyClass& rhs);
private:
    Value* vp
}

MyClass& MyClass::operator=(const MyClass& rhs){
    delete this->vp;
    this->vp = new Value(*(rhs.vp)); // 调用Value 的拷贝构造函数
    return *this; // 见 10
}

// 考虑上面的如果是自我复制会出现什么情况，*(rhs.vp) 将为nullptr,所以拷贝构造函数出现错误

// 一个容易令人想到的解决方法是在赋值函数中进行一次判断比如下面
MyClass& MyClass::operator=(const MyClass& rhs){
    if(this == &rhs){return *this;} // 如果是自我复制就什么都不做 (证同测试)
    delete this->vp;
    this->vp = new Value(*(rhs.vp));
    return *this; 
}

/*
* 更谨慎的讲，这依然存在问题，没有考虑到异常安全性，因为我们不能保证 new的时候会不会出现异常
* 就不如可能出先内存不够的问题，如果出现了，那vp 已经被释放了，前后状态不一致。值得庆幸得是
* 一般operator= 具备异常安全性，通常也具备自我复制安全性，下面是一种解决方法。
*/

MyClass& MyClass::operator=(const MyClass& rhs){
    Value* temp = this->vp;
    this->vp = new Value(*(rhs.vp));
    delete temp;
    return *this
}
/* 
* 代码更新后如果new 抛出异常，vp 前后状态不变，具备异常安全性，并具备自我复制安全性
* 当然也存在弊端，毕竟在自我赋值地过程中没必要如此动作，因此也可以加上证同测试，
* 但是需要你衡量其利弊，因为并不总是自我赋值，证同测试同样影响效率
*/

// 另一个较好的方法是 copy and swap (见29)

```



### 12：复制对象时勿忘其每一个成分

```c++
/* Copy all parts of an object ,这似乎很容易理解，下面是一个例子 */
class Person{
public:
    Person(const Person& per);
    Preson& operator=(const Person& per);
    ...
private:
    std::string name;
}
... ;// 省略其定义

/* 
* 其拷贝构造和拷贝赋值很容易实现, 但是如果在你迷糊的时候在向Person 中添加一个成员变量
* 而你又忘记了更新这两个函数，就会出现局部拷贝的奇观，并且编译器不会提醒！！
* 下面的情形将令人难以察觉
*/

// 假如在继承一个Student 类
class Student: public Person{
public:
    Student(const Student& stu);
    Student& operator=(const Student& stu);
private:
    std::string StuID
};

/* 可能会如此实现拷贝赋值和拷贝构造函数 */

Student::Student(const Student& stu)
    :StuID(stu.StuID)
{
    ...
}

Student& Student::operator=(const Student& stu){
    StuID(stu.StuID);
	return *this;
}

/* 
* 很容看出，虽然派生类的成员已拷贝，但是其基类却没有进行相应的拷贝，有的人可能想在其内部进行赋值操作，
* 但是别忘了，父类的成员是private , 下面是一种实现
*/

Student::Student(const Student& stu)
    :Person(stu), // 调用基类的拷贝构造
	StuID(stu.StuID)
{
    ...
}

Student& Student::operator=(const Student& stu){
    Person::operator=(stu);  // 调用基类的拷贝赋值，对基类部分进行赋值
    StuID(stu.StuID);
	return *this;        
}

/*
* 一个值得注意的点是，不要试图在拷贝构造和拷贝赋值函数中令其中一个调用另一个，
* 如果你发现它们的相同操作过多，你应该将它们提取新的函数中,并由它们共同调用。
*/

```





## 三、资源管理



### 13：以对象管理资源

```C++
/* 利用对象的析构函数来管理资源 */

class MyClass{...};

MyClass* CreateMyClass(); // 一个工厂函数

// 可能如此使用
void f(){
    MyClass* t = CreateMyClass();
    // doSomething
    delete t;
}
/*
* 释放了资源，这看似没有问题，但是doSomething 无法保证是否存在return 语句或者 抛出异常，
* 如果你试图在每一个return 语句和可预测的异常前都加上 delete，是否太麻烦了，
* 利用类的析构自动调用来管理资源是一个不错的选择, 并且C++ 已经提供这种类--智能指针
*/


void f(){
    std::unique_ptr<MyClass> t(CreateMyClass); // 头文件 memory
    // doSomething
}
// 用了智能指针无论doSomething 中是否出现提前的return 或者异常，它都能正确的释放资源

std::shared_ptr; // 另一种智能指针
/*
* 其不同之处就是，unique 类的只能有一个指针指向同一资源，并且无法拷贝和赋值(如果你非要的话也不是不行)
* shared 类允许有多个指针指向向同一资源，其内部是计数，若为0，则delete
*/


/* 值得注意的是智能指针内部使用的是delete 而不是delete[] */
std::share_ptr<int> t(new int[1024]); // 不能完全释放其内存，因为内部是delete
std::unique_ptr<std::string> t(new string[16]); // 同上

/* 有个疑问是C++ 为什不提供new[] 类型的指针指针，其原因是vector 几乎总是可以代替它们 */

```



### 14：在资源管理类中小心 Copying 行为

```C++
/* 
* 在13中我们知道要使用类来进行资源管理，并且提供了智能指针，但是智能指针是表现在 heap-based,
* 资源并不总是 heap-base，因此有时候你应该自已设计资源管理类,例如Mutex 的互斥器对象
*/

void lock(Mutex& pm);   // 为对应资源上锁
void unlock(Mutex& pm); // 为对应资源解锁

/* 下面是一种设计方法 */
class Lock{
public:
    Lock(Mutex& pm)
        :mutexPtr(pm)
    {
        lock(mutexPtr);
    }
    
    ~Lock(){
        unlock(mutexPtr);
    }
private:
    Mutex* mutexPtr;
};

// 客户这样调用
Mutex m;
....;
{
    Lock m1(&m);
    ...; // 区块末尾自动调用其析构函数，从而解锁
}

// 一个可能的情况是，如果Lock 发生的拷贝如何应对
Lock m1(&m);
Lock m2(&m1);

/*
* 禁止复制: 将拷贝函数声明为private (见6),较好的方法是将Lock私有继承于Uncopyable
*/    
class Lock: private Uncopyable{};


/*
* 引用计数法: 利用shared_ptr 智能指针，但是shared_ptr 会在计数器为0时delete 对象，
* 而我门希望的是unlock，幸运的是我们可以自定义shared_ptr 的删除器。
*/

class Lock{
public:
    explicit Lock(Mutex* pm)
        :mutexPtr(pm,unlock) // 将删除器指定为unlock
    {
    	lock(mutexPtr.get()); // 见15
    }
private:
    std::shared_ptr<Mutex> mutexPtr; // 使用智能指针
};
// 以上我们无需再析构函数中unlock,因为我们已经指定了删除器，mutexPtr 会再计数器为0时自动unlock

```



### 15：在资源管理类中提供对原始资源的访问

```C++
/*
* 由于历史原因，APIs往往要求访问原始资源。所以每一个RAII class(资源管理类)应该提供一个
* 获得其原始指针的方法，可以有显示或者隐式转换，但是显示更安全，隐式则方便客户
*/

/* 一般智能指针都提供一个get 方法来显示获取其原始指针 */
class INTER{...};
std::shared_ptr<INTER> p(new INTER());
void func(INTER* p); // 该函数非需要原始指针
func(p.get()); // 如此调用


/* 假如我们自己写的RAII class 同样可以添加一个get 方法，那如何隐式转换呢 */
class INTER_RAII{
public:
    ...
    opertor INTER() const{  // 隐式转换
        return ptr;
    }
    ...
private:
    INTER* ptr;
};

void func(INTER* p); // 该函数非需要原始指针
INTER* p = new INTER();
func(p); // 直接调用即可

/* 
* 隐式调用不太安全的，当将一个资源管理类赋值到给另一个时候，而你错误的将被赋值者声明为原始指针类型
* 编译器不会报错
*/


/* 总之提供原始指针要谨慎小心，比如下面的代码 */
class Date{ // Date 类
public:
    Date(int day):day(day){std::cout<<"Date Constructing..."<<std::endl;}
    ~Date(){std::cout<<"Date Destructing..."<<std::endl;}
    void getDay(){std::cout<<day<<std::endl;}
private:
    int day;
};


class DateRAII{ // Date 的资源管理对象
public:
    DateRAII(Date* pr): pr(pr){
        std::cout<<"DateRAII Constructing..."<<std::endl;
    }
    ~DateRAII(){
        delete pr;
        std::cout<<"DateRAII Destructing..."<<std::endl;
    }

    operator Date*(){  // 隐式
        return pr;
    }
    
    Date* getPr(){return pr;}  // 显示

private:
    Date* pr;
};

DateRAII createDate(int day){
    return DateRAII(new Date(day));
}

int main(){
    /* 由于临时变量没有被及时接受，而是被隐式转换，Date* ，导致DateRaII 被过早的析构，
       因此原始指针其实已经被释放了，所以拿到的是空指针，如果没有转换，而是选择正确的类型
       DateRAII 来接受createDate 的返回值，则不会被过早析构*/
    Date* d1 = createDate(5).getPr(); 
    Date* d2 = createDate(7); // 显示的结果和隐式相同
    d1->getDay();
    d2->getDay();
    return 0;
}

```



### 16：成对使用 new 和 delete 时采取相同的形式

```C++
/*
* 很简单，如果你在new 表达式中使用了[],必须在相应的delete 表达式中使用[]
* 否则，delete 一定不要使用[]
*/

std::string* str1 = new std::string;
std::string* strArr = new std::string[100];

delete str1;
delete[] strArr;

/* 值得注意的是有时候需要使用delete[] 的时候可能不太明显，比如typedef */

typedef std::string strArr[1024]; // strArr 是一个std::string[4] 的别名
std::string* strList = new strArr;

delete[] strList; // 正确的调用delete的方式

```



### 17：以独立语句将 newed 对象置入智能指针

```C++
/* 考虑下面的代码 */

class INTER{}; // 一个平平无奇的类

/* 两个平平无奇的函数 */
int func2(); 
void func1(std::shared_ptr<INTER> pt, int num); 

/* 应该存在如此调用 */
func1(std::shared_ptr<INTER> pt(new INTER()), func2());

/* 那么调用func1 之前需要完成下面工作 */
new INTER();
调用std::shared_ptr<INTER> 构造函数;
func2();
    
/*
* 那么C++　以什么次序执行呢，可以确定的是，INTER 的构造一定在智能指针构造之前，那func2 呢？
* 不幸得是不能确定，这与java 和C# 不同，也许编译器令func2 以第二得顺序执行
*/
new INTER();
func2();
调用std::shared_ptr<INTER> 构造函数;

/*
* 问题是如果func2() 出现异常怎么办，那new 的空间就无法正确回收，而造成内存泄漏
* 所以以独立语句将newed 对象置入智能指针是必要的
*/

std::shared_ptr<INTER> pt(new INTER());
void func1(pt, func2()); 
    
```





## 四、设计与声明



### 18：让接口容易被正确使用，不易被误用

```C++
/*
* 如果你设计的接口被客户所使用，然而却没有达到期望，这个代码不应通过编译，如果通过了编译
* 就该达到客户的需求，欲开发一个容易被正确使用不易被误用的接口，首先应该考虑客户会犯什么样的
* 错误，下面是一个例子
*/

class Date{  // 一个关于日期的类
public:
    Data(int month, int day, int year);
private:
	//...
};

Date d1(30,3,1998); // 显然不符合常理，顺序错了

// 显而易见的是Date 的构造容易被误用，但是编译器不会有任何提示，因为它符合int
// 为此我们可以设计几个外覆类型

struct Day{
	explicit Day(int d):val(d){}
	int val;    
};

struct Month{
	explicit Month(int m):val(m){}
	int val;    
};

struct Year{
	explicit Year(int y):val(y){}
	int val;    
};

/* 如此Date 的构造就可以如此更改 */
Date(const Month& m, const Day& d, const Year& y); // 错误的类型编译器会报错

/* 
* 尽管struct 已经足够好了，但是用class 可能更好，另外一个新的类型被确定，
* 限制其值是合理的，例如Month 
*/

class Month{
public:
    static Month Jan() {return Month(1);}
    static Month Feb() {return Month(2);}
    //...
    static Month Dec() {return Month(12);}
private:
    explicit Month(const int m);
};

Date d2(Month::Dec(),Day(29),Year(2023)); // 如此调用

/* 值得注意的是为什么不如此设计Month class */ 
class Month{
public:
    static Month Jan = Month(1);
    static Month Feb = Month(2);
    //...
    static Month Dec = Month(12);
private:
    explicit Month(const int m);
};

/* 以为类似于 Jan\Feb\Dec 的变量都是non-local static,见4 */


/*
* 任何接口如果企图客户记住某些事情，就是有着不正确使用的倾向，比如一个工厂函数，返回一个
* 原始指针，如果设计者企图让客户记住delete 该对象，那客户就有了犯错的机会
*/

class INTER{...};
INTER* createINTER(); // 工厂函数，返回原始指针企图客户记住delete 它

/* 我们不如直接将返回值类型设置为智能指针 */
// 强迫客户用智能指针类型接收，如果是自己写的RAII，警惕转换无论隐式显示，见15
std::shared_ptr<INTER> createINTER(); 

/* 智能指针被建立起来的时候允许指定特定的删除器 */
std::shared_ptr<INTER> createINTER(){
                                                      // 指定的删除器
    std::shared_ptr<INTER> retVal(static_cast<INTER>(0),deletorOfINTER);
    retVal = ...; // 指向正确的对象
    return retVal;
}

/* 你可能会好奇为什么不在 retVal 建立之初直接指定其原始指针，还要先指定null,在赋值(见26) */

```



### 19：设计 class 犹如设计 type 

```C++
/*

新type的对象应该如何被创建和销毁？
这会影响到你的class的构造函数和析构函数以及内存分配函数和释放函(operator new,operatornew[],
operator delete和operator delete[]---见第8章的设计)，当然前提是如果你打算撰写它们。


对象的初始化和对象的赋值该有什么样的差别？
这个答案决定你的构造函数和数值(assignment)操作符的行为，以及其间的差异。很重要的是别混淆了“初始化”和“赋值”，因为它们对应于不同的函数调用(见4)。


新type的对象如果被 passed by value(以值传递)，意味着什么？
记住，copy构道函数用来定义一个type的pass-by·value该如何实现。


什么是新type的“合法值”？
对class的成员变量而言，通常只有某些数值集是有效的。那些数值集决定了你的class 必须维护的约束条件
(invariants)，也就决定了你的成员函数(特别是构造函数、赋值操作符和所谓"setter”函数)
必须进行错误检查工作。它也影响函数抛出的异常、以及(极少被使用的)函数异常明(exception specifications)。


你的新type需要配合某个继承图系(inheritance graph)吗？
如果你继承自某些既有的classes，你就受到那些classes的设计的束缚，特别是受到“它们的函数是virtual
或non-virtual”的影响(见34、36)。如果你允许其他classes继承你的class，那会影响你所声明的函数
尤其是析构函数——是否为virtual(见7)。


你的新 type需要什么样的转换？
你的type生存于其他一海票types之间，因而彼此该有转换行为吗？如果你希望允许类型r1之物被隐式转换为类型r2之物，就必须在class T1 内写一个类型转换函数(operator T2)或在 class r2内写一个non-explicit-one-argument(可被单一实参调用)的构造函数。如果你只允许explicit构造函数存在，就得写出专门负责执行转换的函数，且不得为类型转换操作符(typeconversion operators)或non-explicit-one-argument构造函数。(条款15有隐式和显式转换函数的范例。)


什么样的操作符和函数对此新type而言是合理的？
这个问题的答案决定你将为你的class声明哪些函数。其中某些该是member函数，某些则否(见条款23，24，46)。


什么样的标准函数应该驳回？
那些正是你必须声明为private者(见条款6)。


谁该取用新 type 的成员？
这个提问可以帮助你决定哪个成员为public，哪个为protected，哪个为private。它也帮助你决定哪一个 classes 和/或 functions 应该是friends，以及将它们嵌套于另一个之内是否合理。


什么是新type的“未声明接口”(undeclared interface)?
它对效率、异常安全性(见条款29)以及资源运用(例如多任务锁定和动态内存)提供何种保证？你在这些方面提供
的保证将为你的class实现代码加上相应的约束条件。


你的新type有多么一般化？
或许你其实并非定义一个新type，而是定义一整个types家族。果真如此你就不该定义一个新class，
而是应该定义一个新的class template。




你真的需要一个新type吗？
如果只是定义新的derived class以便为既有的class添加机能，那么说不定单纯定义一或多个non-member函数或
templates，更能够达到目标。



这些问题不容易回答，所以定义出高效的classes是一种挑战。然而如果能够设计出至少像C++内置类型一样好的
用户自定义(user-defined) classes，一切汗水便都值得。


请记住: 
Class的设计就是type的设计。在定义一个新type之前，请确定你已经考虑过本条款覆盖的所有讨论主题。

*/

```



### 20：宁以 pass-by-reference-to-const 替换 pass-by-value

```C++
/*
* 缺省的情况下C++ 以by-value 方式传递对象至函数 
* 如果你传递的类型不是内置类型，以及STL 迭代器和函数对象，你应该pass-by-reference-to-const 
* 下面是的例子说明为什么如此
*/

class Person{
public:
    //...
private:
    std::string name;
    std::string sex;
};

class Student: public Person{
public:
    //...
private:
    std::string schoolName;
    std::string schoolAddress;
};

bool isValidStudent(Student stu); // 考虑这个函数
/*
* 当isValidStudent 被调用就会拷贝一个Student 副本，也许你以为只是执行了一次构造和一次析构而已
* 事实并非如此，Student 被拷贝，必须构造其本身以及基类，还有string 对象，其负担是比较大的
* 因此这种情况下pass-by-reference-to-const 远比 pass-by-value高效
*/

bool isValidStudent(const Student& stu); // 没有任何构造产生


/*
* 并且pass-by-reference-to-const 可以避免对象切割问题，当一个派生类对象被以by-value 方式
* 传递并被视为一个基类对象，基类的拷贝构造函数就会被调用，而派生类的那些特征就如同被切割了一般
* 仅仅留下了一个基类对象
*/

Student s1;
bool isValidStudent(Person stu){ // stu 就是一个拷贝了以s1 的基类的副本，没有student特征
    //...         
}
/* 
* 当然最直观的是virtual 函数部分，如需要调用虚函数，stu 只能调用父类版本，尽管由s1拷贝而来，
* 而pass-by-reference-to-const 就完美解决了该问题(不演示了，因为引用的底层是指针)
*/

```



### 21：必须返回对象时，别妄想返回其 reference

```C++
/* 有的人知道了引用的好处之后，就试着消除by-value 的方式，但是这是不对的 */
/* 这是一个例子 */
class Rational{ // 一个关于有理数的类
public:
    Rational(int Numerator, int denominator); // 分子和分母
private:
    int n;
    int d;
    /* 一个友元函数，它试图返回一个引用 */
    friend const Rational& operator*(const Rational& lhs, const Rational& rhs){
        Rational result(lhs.n * rhs.n, lhs.d * rhs.d);
        return result; // 返回了一个局部的引用，但是当客户拿到的时候已经为空
    }
};

/*
* 可能有人将上面的代码补充完整后，运行没有发生什么问题，但是这是编译器优化的结果
* 你使用引用可能是为了减少构造函数的调用，但是上面的代码并没有达到需求。
* 试图返回一个reference 指向某个local 对象都将一败涂地，如果没有编译器优化...
* 那我返回一个指针指向的local 对象不就行了？
*/

friend const Rational& operator*(const Rational& lhs, const Rational& rhs){
    Rational* result = new Rational(lhs.n * rhs.n, lhs.d * rhs.d);
    return *result; // 将指针解引用返回出去
}

/*
* 不变的事实是它仍然调用了构造函数，变得是它的返回确实不是空，但是谁来delete 这个对象
* 假如你谨慎的调用，或者是编译器拉你一把，但是不保证每次都成功，比如
*/

Ration r4 = r1 * r2 * r3;

/* 有人提议用static,就比如 */

friend const Rational& operator*(const Rational& lhs, const Rational& rhs){
    static Rational result;
    result = ...; // 放入结果
    return result; 
}

// 好像很不错,似乎解决了返回delete 的问题，但是另一个问题又来了，下面的代码更直观的表现出来

if((r1 * r2) == (r3 * r4)){ // 你可以振臂高呼，“我发现了一个永真式！”
    // ...
}

// 为什么？因为前者的相乘改变的是static,后者同样，都是一个变量，自我相等这没问题...

// 所以为什么不这样写？ 完美解决上面的所有问题
friend const Rational operator*(const Rational& lhs,const Rational& rhs){
    Rational result(lhs.n * rhs.n);
    return result;
}

```



### 22：将成员变量声明为 private

````C++
/* 
* 将成员变量声明为private ,可赋予客户访问的一致性，无需记忆是否加小括号，可以更精确的访问
* 比如，只可读、只可写、可读可写。
* 更重要的是封装，如果是通过函数进行访问，那么以后的替换更新将更容易，使用者不会察觉，
* 将数据成员隐藏在函数之下，提供更多灵活性
*
*
* protected 不比public 更具有封装性
*/


````



### 23：宁以 non-member、non-friend 替换 member 函数

```c++
/* 直入正题，看看下面的一个关于浏览器的类 */

class WebBrowser{
public:
    //...
    void clearCache();
    void clearHistory();
    void removeCookies();
    void clearEverything(); // 用来调用上面的三个函数
};

// clearEverthing 也可以用一个non-member 函数实现

void clearEverthing(WebBrowser& wb){
    wb.clearCache();
    wb.clearHistory();
    wb.removeCookies();
}

//那么哪一个比较好呢？(标题已经告诉...)
/*
* 面向对象守则要求，数据以及操作数据的那些函数应该被捆绑在一起，似乎member 函数是更好的选择
* 但是并非如此，对于clearEverthing 而言，non-member 的封装性优于 member.
* 提供non-member 函数可允许对 WebBrowser 相关技能有较大的包装弹性，而那最终导致较低的
* 编译依存性。那么，为啥嘞
*/

/*
* 为什么封装，如果某些东西被封装，那么它就不可见，我们就有较大的弹性去修改它，
* 封装性并不是把函数一股脑的全塞进类里，而是尽可能减少对私有成员的访问，因此没有必要增加一个
* 可以访问private 的函数(尽管在里面没有显示的访问private)
*/

/* 这样写更加自然，并且可以减少编译依赖性 */
namespace WebBrowserStuff{
    class WebBrowser{};
    void clearEverthing(WebBrowser& wb);
    //...
}


// 下面说明了这种方法的优越性

/****************************** WebCore.h(核心头文件)*************************/
namespace WebBrowserStuff{
    class WebBrowser{};
    void CoreFunc(WebBrowser& wb); // non-member
    //...
}
/****************************** WebCore.h(核心头文件)*************************/










/****************************** WebClear.h(清理头文件)*************************/
#include "WebCore.h"

namespace WebBrowserStuff{
    void clearEverthing(WebBrowser& wb); // non-member
    //...
}
/****************************** WebClear.h(清理头文件)*************************/


// 假如我们在我没的程序中没有用到WebClear,我没就没有必要include 它，但是不妨碍我们用WebBerser


```



### 24：若所有参数皆需类型转换，请为此采用 non-member 函数

```C++
/* 
* 令class 支持隐式转换通常比较糟糕，但是，有时候又是合理的，比如C++ 内置的double 到int
* 我们以一个有理数的类为例，将int 隐式转换成int 是非常合理的
*/

class Rational{
public:
    Rational(int numerator=0,int denominator=1); // 有缺省值，并且允许隐式转换
    ~Rational();
    //...
private:
    int numerator;
	int denominator;
    //...
};



/* 如果要支持运算，我们应该重载运算符，但是写在何处呢，本这面向对象的原则就写在class 中吧 */

class Rational{
public:
    // 或许你可以考虑一下返回结果为 const-by-value,参数是 by-reference-to-const
    const Rational operator*(const Rational& rhs) const; 
    //...
};

// OK 了试着调用一下

Rational r1(1,2);
Rational r2(1,3);
Rational r3 = r1 * r2; //  r1.operator*(r2) , 没有问题

// 试试对int 的隐式转换

Rational r4 = r1*5; // r1.operator*(Rational(5,1)) 没毛病分母缺省为1

Rational r5 = 6 * r2; // 错误！6.operator*(const Rational&),没有找到该函数

/*
* 其实编译器会企图寻找全局的 
* const Rational operator*(const Rational& lhs,const Rational& rhs);
* 但是没有...
* 将它声明为non-member ,允许在每一个参数上进行隐式转换
*/

const Rational operator*(const Rational& lhs,const Rational& rhs);

/*
* 值得注意的是她是否应该是个friend,切记member 反面是non-member,有些人认为如果
* 它不是member,它就应该是一个friend，这就牵强了
*/

/*
* 当从Object-Oriented C++ 跨进 Template C++ 让Rational 成为一个 class template
* 又会怎么样呢？(见46)
*/
```



### 25：考虑写出一个不抛出异常的 swap 函数

```C++
/**************************** 声明 ***********************************/
/* C++11 之前swap是通过标准的交换算法实现的，但是C++11之后用的是std::move,  */
/* 并明确声明为noexcept,所以书中的问题已经不存在了，我们旨在讨论其中的一些思想，*/
/* 了解C++11之前是如何处理的。                                          */
/********************************************************************/


/* 缺省的情况下有std::swap  完成交换动作 */
namespace std{
    template<typename T>
    void swap(T& a,T& b){
        T temp(a);
        a = b;
        b = temp;
    }
}


/* 考虑下面的类 */


class WidgetImpl{
public:
    //...
private:
    int a,b,c; // 可能有很多数据
    std::vector<double> vb;
};

class Widget{
public:
    Widget(const Widget& rhs);
    Widget& operator=(const Widget& rhs){
        //....
        *pImple = *(rhs.pImple); // 细节见 10，11，12
        //...
        return *this;
    }

private:
    WidgetImpl* pImple; // 数据存放之处
};

// 如果调用std::swap 会发生什么,复制三次widget,复制三次widgetImpl对象....

/* 我们从上帝视角可以得知，其实我们只不过需要交换一下WidgetImpl 指针即可，因此需要特化std::swap */

namespace std{
    template<>
    void swap(Widget& a,Widget& b){
        swap(a.pImpl,b.pImpl); // std::swap 能直接操作private?,因此编译错误
    }
}

/* 故我们可以为Widget 提供一个swap的member 函数 */

void Widget::swap(Widget& rhs){
    using std::swap; // 将 std::swap 暴露出来
    swap(this->pImpl,rhs.pImpl);
}

namespace std{
    template<>
    void swap(Widget& a,Widget& b){
        a.swap(b);
    }
}

/* 
* 上面的做法不仅可以通过编译，还与STL容器有一致性，因为所有的STL容器也都提供有public swap
* 和std::swap 的特化版本   
*/

/* 让我们深入问题，如果Widget 和 WidgetImpl 都是Template Class 呢 */
template<typename T>
class Widget{...};
template<typename T>
class WidgetImpl{...};

/* 那我们如此写 */
namespace std{
    template<typename T>
    void swap(Widget<T>& a,Widget<T>& b){
        a.swap(b);
    }
}
/*
* 一般而言是合法的，重载function template,但是 std 是一个特殊的命名空间，std 内容完全由C++
* 标准委员会决定，他们禁止我们膨胀那些声明好的，请不要添加任何新的东西到std;
*/

/* 解决方法很简单，那就不写在std::swap 内呗 */

namespace WidgetStuff{
    template<typename T>
    class Widget{...};
    template<typename T>
    class WidgetImpl{...};
    
    template<typename T>
    void swap(Widget<T>& a,Widget<T>& b){
        using std::swap; //将std::swap 暴露出来，令编译器合理选择
        a.swap(b); // 试着调用 更高效的，比如在std::swap 的特化版本
    }
}


/*
* 首先，如果缺省的swap 对你的class 或class Template 效率可接受，那么就什么都不做
* 否则
* 1、提供一个public swap 函数，让它高效的置换两个对象
* 2、在你的class 或 template 的命名空间内提供一个non-member swap,并调用上述swap 成员函数
* 3、如果你正编写的class(非template class)，为它特化 std::swap,令其调用member-swap
* 最后，如果你调用swap,请包含一个using声明式，以便让std::swap在你的函数内曝光可见，然后
* 赤裸的调用swap
*/

/*
* 成员版的 swap 绝不可抛出异常(不可实施于non-member,见29)
*/

```





## 五、实现



### 26：尽可能延后变量定义式的出现时间

```C++
/* 也许你会声明一个你用不到的变量，下面代码是个例子 */

/* 这是一个对密码进行加密的函数 */
std::string encryptPassword(const std::string& password){
    using namespace std;
    string encrypted; // 过早的定义
    if(password.size()<MinimumPasswordLength){ // 如果密码达不到要求抛出异常
        throw logic_error("Passworld is too short");
    }
    //.....
    return encrypted;
}

/* 上面的示例中，如果抛出异常，那么encrypted 不就白白构造了吗，延后它的定义直到需要用到 */

std::string encryptPassword(const std::string& password){
    using namespace std;
    if(password.size()<MinimumPasswordLength){ // 如果密码达不到要求抛出异常
        throw logic_error("Passworld is too short");
    }
    string encrypted; // 只有需要的时候才被定义
    encrypted = password;
    encrypt(encrypted); // 将密码加密
    //.....
    return encrypted;
}

/* 上面的encrypted 在创建的时候调用default 构造，然后再赋值拷贝，不如直接拷贝构造 */

std::string encryptPassword(const std::string& password){
    using namespace std;
    if(password.size()<MinimumPasswordLength){ // 如果密码达不到要求抛出异常
        throw logic_error("Passworld is too short");
    }
    string encrypted(password); // 省去赋值拷贝，直接拷贝构造
    encrypt(encrypted); // 将密码加密
    //.....
    return encrypted;
}

/* 但有时候也许会有分歧，就比如在循环中，就需要具体分析 */

Widget w;
for(int i=0;i<n;++i){
    w = func(i);
    //....
}


for(int i=0;i<n;++i){
    widget w(func(i));
    //....
}

/*
* 第一种:  一次构造 + 一次析构 + n次赋值
* 第二种:  n次构造  + n次析构
* 很明显，如果很小的情况下，没有太大意义，但是n很大，就要考虑
* n次赋值 和  n次构造+n次析构 ，二者的成本
* 除非你知道前者的成本更低，否则在高度敏感的代码区域中应该使用第二种
*/

```



### 26：尽量少做转型动作

````C++
/*
* C风格强制转换：
* 	(T)expression 或者 T(expression) // 都一样，将expression 转换成T
* 
* C++风格：
*    const_cast<T> (expression) :用于移除对象的常量限制。这是唯一能做到这一点的c++风格的强制转换
*
*    dynamic_cast<T> (expression) :用于执行“安全的向下转换”，即确定对象是否属于继承层次结构中
*       的特定类型。这是唯一不能使用旧式风格完成的转换。它也是唯一一种可能有较大运行时开销的类型转换
*
*    reinterpret_cast<T> (expression) :用于底层强制转换，其结果依赖于实现(即不可移植)，
*        例如将指针强制转换为int类型。这种类型转换在底层代码之外应该很少见。
*
*    static_cast<T> (expression): 用来强迫隐式转换
*				单向：例如，非const对象到const对象，int到double的转换等等
*				双向：指向类型化指针和void*指针，基类的指针和派生指针等等
*/

/* C 风格依然合法，但是C++ 新式的更受欢迎，容易辨认。*/

class Widget {
public:
    explicit Widget(int size);  // 禁止隐式转换
    ...
};
void doSomeWork(const Widget& w);
doSomeWork(Widget(15)); 		//函数式，从int创建Widget
doSomeWork(static_cast<Widget>(15)); // C++风格，从int创建Widget

/* Widget(15); 是转型吗？，感觉不像...,因此这种情形下很少使用static_cast<Widget>(15) */


/* 转型什么都没做吗，只是编译器将其视作另一种类型看待？ */
class Base { ... };
class Derived : public Base { ... };
Derived d;
Base* pb = &d; // 隐式转换：Derived* 转换成 Base*

/* 
* 建立了一个基类指针指向一个派生类对象，但是这两个指针的值并不相同，这种情况下会有一个偏移量(offset) 
* 在运行期间被施行于派生类指针身上，以取得正确的基类指针值。
* 指针之间如此，更不用说int 到 double 之间转换的了
*/

/*
* 派生类SpecialWindow 调用virtual 函数时需要先调用基类Window对应的函数，比如 onResize()
*/


class Window { // 基类
public:
    virtual void onResize() { ... } // onResize的基类实现
    ...
};
class SpecialWindow : public Window { // 派生类
public:
    virtual void onResize() { // // onResize的派生类实现;
        static_cast<Window>(*this).onResize(); // 将 *this 转换为 Window,
        				// 然后调用它的onResize，这不管用!
        ... // 这里做派生类的特有任务
    } 
    ...
};

/*
* static_cast<Window>(*this).onResize(); 并不能达到预期的效果，因为这种转换会为*this的基类部分
* 创建一个新的临时副本，然后在副本上调用onResize，因此不要让编译器将*this看作是其基类部分的对象
*/

Window::onResize(); // 正确的做法，即达到预期，又简单，何乐而不为


/* 
* 尽量避免使用 dynamic_cast，如果你想使用dynamic_cast 那大概是你想调用只有派生类才有的函数
* 但是你只有一个指向基类的指针，像下面这样
*/

class Window { ... }; // base class

class SpecialWindow : public Window { //derived class
public:
    void blink();// 假设只有SpecialWindow才有这个功能
    ...
};

typedef std::vector<std::shared_ptr<Window> > VPW; // 一个储存基类智能指针的vector的别名
VPW winPtrs;
...
for (VPW::iterator iter = winPtrs.begin(); iter != winPtrs.end(); ++iter) {
    // 不良代码:使用了dynamic_cast
    if (SpecialWindow* psw = dynamic_cast<SpecialWindow*>(iter->get()))
        psw->blink();
}


/* 为了避免这样的代码，我们一开始就应该如此设计 */
typedef std::vector<std::shared_ptr<SpecialWindow> > VPW;


/* 你可能会说，我本来是想储存Window 的任意派生类，但是如此一改....,那我们就从基类着手修改 */

class Window {
    //...
    virtual void blink(){} // 在当下情形中这可能是合理的，(见34)
    //...
};


/* 最后一种情况 连串dynamic_cast 是你必须要避免的 */

class Window { ... };
... // 一些派生类的定义
typedef std::vector<std::tr1::shared_ptr<Window> > VPW;
VPW winPtrs;
...
for (VPW::iterator iter = winPtrs.begin(); iter != winPtrs.end(); ++iter)
{
    if (SpecialWindow1* psw1 =dynamic_cast<SpecialWindow1*>(iter->get())) {...}
    else if (SpecialWindow2* psw2 =dynamic_cast<SpecialWindow2*>(iter->get())) {...}
    else if (SpecialWindow3* psw3 =dynamic_cast<SpecialWindow3*>(iter->get())) {...}
    ...
}
/*
* 这样的C++生成的代码既大又慢，而且很脆弱，因为每次Window类层次结构发生变化时，
* 都必须检查所有这些代码以确定是否需要更新....
*/

````



### 28：避免返回 handle 指向对象的内部成分

````c++
/* handle 句柄 */
/* 考虑一个关于矩阵的类，包含数据左上角和右下角坐标的数据 */

class Point { // 表示点的类
public:
	Point() {};
	Point(int x, int y) :_x{x},_y{y}
	{
	}
	
	void setX(int newVal) {
		_x = newVal;
	}
	void setY(int newVal) {
		_y = newVal;
	}

	std::string toString() const {
		std::string result = "(x:" + std::to_string(_x) + "," + std::to_string(_y) + ")";
		return result;
	}
private:
	int _x = 0;
	int _y = 0;
};



struct RectData { // 矩形点的结构体
	Point ulhc; // 左上角点
	Point lrhc; // 右下角点
};




class Rectangle {
public:
	Rectangle(){}
	Rectangle(const Point& p1, const Point& p2) {
		RectData rectData;
		rectData.ulhc = p1;
		rectData.lrhc = p2;
		pData = std::make_shared<RectData>(rectData);
	}
	Point& upperLeft() const { return pData->ulhc; } // const 函数返回一个non-cosnst引用
	Point& lowerRight() const { return pData->lrhc;}
	void print() const{
		std::cout<<pData->ulhc.toString()<<std::endl;
		std::cout << pData->lrhc.toString() << std::endl;
	}
private:
	std::shared_ptr<RectData> pData;  // 一个指向结构体的智能指针
};


/* 考虑下面的调用 */

Point coord1(0, 0);
Point coord2(100, 100);
const Rectangle rec(coord1, coord2); // 一个const 类被创建	
rec.upperLeft().setX(50); // 没有编译错误，你是否发现了不合理之处

/*
* 返回的handle(引用)，使我们可以修改const 类，准确的说是可以调用非const函数
* 其实没有问题，编译器是bitwise constness,因此我们需要一些改动
*/

const Point& upperLeft() const { return pData->ulhc; } 
const Point& lowerRight() const { return pData->lrhc;}

/* 这样的话调用原来的代码就会报错 */

/* 在上面的基础上我添加了一个non-member 函数 */

const Rectangle func(){ // 返回一个 value 而不是 reference
    Point coord1(0, 0);
	Point coord2(100, 100);
	const Rectangle rec(coord1, coord2);
    return rec;
}

/* 如此调用 */
Point* temp = &(func().upperLeft()); // 调用之后，temp会指向null

/* 因为没有参数去接受 func ，语句结束后func()  返回被销毁，导致Point 的析构 */
// 如此调用
Rectangle t = func();
Point* temp = &(t.upperLeft());

/*
* 避免返回指向对象内部的句柄(引用、指针或迭代器)。遵守这个原则将会增加了封装性，
* 帮助const成员函数保持const行为，并可以尽量避免发生悬空句柄的创建
*/

````



### 29：为“异常安全”而努力是值得的

```C++
/*
* 假设我们有一个类来表示带有背景图像的GUI菜单。这个类被设计为支持多线程环境，
* 所以它有一个互斥量来控制并发
*/

class PrettyMenu {
public:
    ...
    void changeBackground(std::istream& imgSrc); // 改变背景图像
    ... 
private:
    Mutex mutex; 	// 这个对象的互斥量 
    Image* bgImage; 	// 当前背景图像
    int imageChanges; // 图像被改变的次数
};

void PrettyMenu::changeBackground(std::istream& imgSrc)
{
    lock(&mutex); 	// 取得互斥量
    delete bgImage; 	// 去除旧的背景
    ++imageChanges; 	// 更新图像变化计数
    bgImage = new Image(imgSrc); // 安装新的背景
    unlock(&mutex); 	// 释放互斥量
}

/* 
* 经过之前的学习，很容易就看出上面代码的问题
* 首先，如果锁在 unlock 之前抛出异常，就会造成严重问题，因为这个这个资源被永久的锁上了
* 而它的钥匙丢了，因此应该利用资源管理类将其管理起来，或者使用智能指针，自定义其删除器
* 其次，如果new 的时候抛出异常，数据就已经被 delete，这个问题可以运用copy的方式改进,
* 或者使用智能指针的 reset
*/


class PrettyMenu {
    //...
    std::shared_ptr<Image> bgImage; // 更改为智能指针类型
    //...
};


void PrettyMenu::changeBackground(std::istream& imgSrc)
{
    Lock m1(&mutex); 	// 取得互斥量,利用资源管理类管理起来
    bgimage.reset(new Image(imgSrc)); 
	++imageChanges; // 更新图像变化计数
}

/*
* 上面我没有用delete ，因为这个动作已经由智能指针内部处理了，并且删除对象只发生在对象成功之后，
* 也就是说，如果没有创建成功(抛出异常)，原来的资源不会发生变化
*/

/*
* 具备异常安全性的函数，提供以下三种保证之一：
* 基本承诺：如果抛出异常，程序中的所有内容都保持有效状态。
* 强烈保证：如果抛出异常，程序的状态不会改变（要么全部成功，要么就像没发生过）。
* 不抛出异常（noexcept）保证：它们总是按照承诺的方式执行。内置类型(例如整型、指针等)上的
*               所有操作都是 noexcept(即提供noexcept承诺)。这是构建异常安全代码的关键组成
*/

/* 另一个常用于提供强烈保证的方法是我们所提到过的 copy and swap */

struct PMImpl { 
    std::shared_ptr<Image> bgImage; 
    int imageChanges; 
};

class PrettyMenu {
    ...
private:
    Mutex mutex;
    std::shared_ptr<PMImpl> pImpl;
};
void PrettyMenu::changeBackground(std::istream& imgSrc)
{
    using std::swap; // 见25
    Lock ml(&mutex); // 获取互斥量
    std::shared_ptr<PMImpl> pNew(new PMImpl(*pImpl));// 复制obj. data
	pNew->bgImage.reset(new Image(imgSrc)); // 改变副本
    ++pNew->imageChanges;
    swap(pImpl, pNew); // 交换数据
} // 释放互斥量


/* copy and swap 虽然安全，但是有时候也必须考虑代码执行效率等方面 */

```



### 30：透彻了解 inlining 的里里外外

```C++
/*
* 内联函数背后的思想是用它的代码体替换对该函数的每次调用，大多数是编译时的行为。
* 这可能会增加目标代码的大小（如果函数特表小还可能减少目标代码的大小）。它看上去，用起来都是函数，
* 还避免了起函数调用的开销。
*/

/* 
* 声明一个函数为inline 是向编译器申请，而不是强制，是否inline 取决于编译器 
* 申请分为隐式和显示，一般在一个类中的直接定义的成员函数将视为隐式申请
*/

class MyClass{
    // ...
    void func(){   // 隐式申请 inline
        return this.x;
    }
    // ...
};


/*
* 如果函数过于复杂，编译器会拒绝内联请求(例如，那些包含循环或递归的函数)；由于编译时无法确定，
* 所有的虚函数都不允许内联。
*/


/* 如果程序需要内联函数的地址，编译器通常必须为其生成函数体 */


inline void f() { ... } 	// 假定编译器愿意内联函数f的调用
void (*pf)() = f; 		// pf 指向f
...
f(); // 这个调用会被内联，因为它是一个“普通的”

pf(); // 这个调用可能不会，因为它是通过函数指针


/* 构造和析构函数通常不适合inline,尽管看起来适合但是其内部的实现还是颇为复杂的 */



/*
* 谨慎的inline,一旦函数被inline，嵌入到程序本体，一旦需要改变该函数，那所有用到该函数的客户端
* 都需要重新编译。如果函数是一个 non-inline 函数，一旦它有所修改，那客户端只需要重新连接即可
* 如果采用动态连接，甚至不知不觉就被程序吸纳
*/

/*
* 将大多数内联操作限制为小型的、频繁调用的函数。这有助于调试和二进制程序的升级，
* 最大限度地减少潜在的代码膨胀，并最大限度地提高程序速度。
*/

```



### 31：将文件间的编译依存关系降至最低

```C++
/*
* 假设你对C++ 程序的某个实现文件(不是接口)做出了轻微的修改，而且只改变private 部分
* 你以为只需要编译一个文件，然而却不是这样
*/

#include <string>
#include "date.h" 
#include "address.h"

class Person {
public:
    Person(const std::string& name, const Date& birthday,const Address& addr);
    std::string name() const;
    std::string birthDate() const;
    std::string address() const;
    ...
private:
    std::string theName; 	// 实现细节
    Date theBirthDate; 	// 实现细节
    Address theAddress; 	// 实现细节
};

/*
* 如果上面的头文件中有任何一个被修改，或者这些头文件所依赖的其他头文件被修改，那么每一个
* 含入Preson Class 的文件都需要重新编译，任何使用Preson class 文件也必须重新编译
* 这无疑是灾难的
*/


/* 这样前置声明可行吗 */

namespace std {
    /* string不是一个类，它是一个typedef(basic_string<char>的别名) */
    class string; // 前置声明(不正确)
}  
class Date; 	// 前置声明
class Address; // 前置声明
class Person {
public:
    Person(const std::string& name, const Date& birthday,const Address& addr);
    std::string name() const;
    std::string birthDate() const;
    std::string address() const;
    ...
};

int main()
{
    int x; 		// 定义一个int（内置类型大小是明确的）
    Person p(params); // 定义一个Person（需要通过数据成员的大小，计算所需内存）
    Person *p2; 	// 定义一个指向Person对象的指针（指针也是内置类型）
    ...
}

/*
* 更重要的：前置声明的困难在于编译器在编译期间需要知道对象的大小。
*/


/* 可以采用pimpl （pointer to implemention）方法，“将对象实现细节隐藏在指针背后” */


/*============================= person.hpp =======================================*/
#include <memory>
#include "personimpl.hpp"

class Person{
public:
    Person();
    Person(const std::string& name);
    ~Person();
    const std::string& getName() const;
private:
    std::shared_ptr<PersonImpl> pImpl; // 一个pImpl指针指向用于实现的类
};
/*================================================================================*/





/*=================================== person.cpp =================================*/
#include <iostream>
#include "person.hpp"

// person构造函数对应的用来调用personImpl的构造函数
Person::Person():pImpl(new PersonImpl()){}
Person::Person(const std::string& name):pImpl(new PersonImpl(name)){}

// 析构就没必要了，因为用的是智能指针
Person::~Person(){}

const std::string& Person::getName() const{
    return this->pImpl->getName(); // 它们用于一样的接口
}
/*================================================================================*/




/*=============================== personimpl.hpp =================================*/
#include <string>

class PersonImpl{   // 这里面放的是数据，和真正的person 函数实现
public:
    PersonImpl();
    PersonImpl(const std::string& name);
    ~PersonImpl();
    const std::string& getName() const;
private:
    std::string _name;
};
/*================================================================================*/




/*=============================== personimpl.cpp =================================*/
#include "personimpl.hpp"

PersonImpl::PersonImpl(){}
PersonImpl::PersonImpl(const std::string& name):_name(name){}
PersonImpl::~PersonImpl(){}

const std::string& PersonImpl::getName() const{
    return this->_name;
}
/*================================================================================*/






/* Handle类方法的另一种选择是让Person成为一种特殊的抽象基类，称为接口类 */

/*这个Person类作为基类提供接口接口，使用该类型的引用或指针，指向采用工厂设计模式创造的派生类对象 */


class Person {
public:
    virtual ~Person();
    virtual std::string name() const = 0;
    virtual std::string birthDate() const = 0;
    virtual std::string address() const = 0;
    //...
    
    // 工厂函数,静态的
    static std::shared_ptr<Person> // 返回类型为shared_ptr 
    create(const std::string& name,  const Date& birthday,  const Address& addr);  

};


class RealPerson : public Person {
public:
    RealPerson(const std::string& name, const Date& birthday, const Address& addr)
        : theName(name), theBirthDate(birthday), theAddress(addr)
    {}
    virtual ~RealPerson() {}
    std::string name() const; 		//  这里省略了实现细节 
    std::string birthDate() const; 	
    std::string address() const; 		
private:
    std::string theName;
    Date theBirthDate;
    Address theAddress;
};


std::shared_ptr<Person> 
Person::create(const std::string& name,const Date& birthday,const Address& addr)
{
    // 虽然智能指针的定义类型为Person , 但是原始指针真正指向RealPerson
 return std::shared_ptr<Person>(new RealPerson(name,birthday,addr));
}


std::string name;
Date dateOfBirth;
Address address;
...
// 创建一个支持Person接口的对象(create 是静态函数，无需实例化)。pp的原始指针为RealPerson
std::shared_ptr<Person> pp(Person::create(name, dateOfBirth, address));
...
std::cout << pp->name() // 通过Person接口使用对象
        << " was born on "  << pp->birthDate()
        << " and now lives at "<< pp->address();
... 

    
    
/*  
* 既然减少了编译的依存性，难道这两种方法没有带来额外的开销吗，怎么可能，Person和PersonImpl
* 中带来的每次访问的间接性，以及PersonImpl必须初始化在heap
* Person和RealPerson中每次调用的虚函数表查询
* 但是，不妨碍我们使用它们，在程序发展的过程中使用它们以求实现码变化时对其客户端带来最小冲击
*/
    
```
