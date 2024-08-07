# Effective C++ 下





## 六、继承与面向对象设计



### 32：确定你的 public 继承塑模出 is-a 关系

````C++
/*
* 公共继承的意思是“is-a”。虚函数意味着“接口必须继承”，而非虚函数意味着“接口和实现都必须继承"  
*/

/*
* 不多赘述，书中无论是企鹅是一种鸟，还是正方形是矩形的例子中，都说明设计的时候需要谨慎
*/


/*
* public继承意味着“is-a”。适用于基类的所有内容也必须适用于派生类，因为每个派生类对象都是基类对象。
*/

````



### 33：避免遮掩继承而来的名称

```c++
/* 有点像函数变量作用域，继承里的名称，实际工作的方式是，派生类的作用域嵌套在其基类的作用域内，考虑下面的代码*/

class Base { // 基类
private:
    int x;
public:
    virtual void mf1() = 0;
    virtual void mf2();
    void mf3();
    //...
};

class Derived : public Base { // 派生类
public:
    virtual void mf1();
    void mf4();
    //...
}

/* 考虑派生类成员函数调用mf4 */
void Derived::mf4(){
    ...
    mf2();
    //...
}
/* 首先在函数内寻找，然后在派生类覆盖的作用域寻找，在之后先基类内寻找(找到了)，最后global */



/* 在次考虑下面的代码 */

class Base {
private:
    int x;
public:
    virtual void mf1() = 0;
    virtual void mf1(int);
    virtual void mf2();
    void mf3();
    void mf3(double);
    //...
};

class Derived : public Base {
public:
    virtual void mf1();
    void mf3();
    void mf4();
    //...
};

/* 当我们如此调用 */
Derived d;
int x;
//...
d.mf1(); // 没问题, 调用 Derived::mf1
d.mf1(x); // 错误! Derived::mf1遮掩Base::mf1
d.mf2(); // 没问题, 调用 Base::mf2
d.mf3(); // 没问题, 调用 Derived::mf3
d.mf3(x); // 错误! Derived::mf3遮掩 Base::mf3

/* 尽管我的派生类是public继承于基类(is-a关系)，但是我的派生类却用不了基类的方法，下面下面是解决办法 */


class Derived : public Base {
public:
    using Base::mf1; // 让Base中所有名为mf1和mf3的东西
    using Base::mf3; // 在Derived的作用域中可见(并且是public)
    virtual void mf1();
    void mf3();
    void mf4();
    //...
};

/* 如此一来下面的调用就没有问题了 */
Derived d;
int x;
...
d.mf1(); 	// 调用 Derived::mf1
d.mf1(x); 	// 调用 Base::mf1
d.mf2(); 	// 调用 calls Base::mf2
d.mf3(); 	// 调用 Derived::mf3
d.mf3(x); 	// 调用 Base::mf3


/* 
* 值得注意的是在派生类中声明 using Base::mf,是开放基类所有命名mf的函数，但是有时候我们只需要用到
* 其中的一个(这样就破环了is-a的关系，但是private下是有意义的)，可以使用inline转发函数
*/

class Derived : private Base {
public:
    virtual void mf1(){ // 转发函数，隐式的inline申请
        Base::mf1();
    } 
    ...
};

/*
*派生类中的名称隐藏基类中的名称。在公共继承下，这是不可取的。
*要使隐藏的名称再次可见，请使用using声明或转发函数。
*/



/* 有一点我需要补充,仔细观察下列代码 */

class Base{
public:
    virtual void func(); // 
};


class Derived: public Base{
public:
    void func(int x);
};

/* 
* 这两个函数是不具有多态性的，如果你不把Base::func()在派生类中暴露出来Base::func()就会被遮盖，
* 它们之间的关系是重载，不是虚函数哪种覆盖关系
*/

/*
* 覆盖（override）是指派生类中的函数重新定义了基类中的虚函数，而且函数签名（参数列表和返回类型）完全匹配。
* 在这种  情况下，派生类的函数将覆盖基类的虚函数，而且通过基类指针或引用调用该函数时将动态绑定到派生类的实现。

* 重载（overload）是指在同一个作用域内有多个同名函数，但它们的参数列表不同。在你的情况下，
* 基类的 func 和派生类的 func 具有不同的参数列表（基类的是无参函数，派生类的是带有一个整数参数的函数），
* 因此它们是重载关系，而不是覆盖关系。这意味着这两个函数可以在同一个类中共存，而不会产生二义性。

* 要注意的是，派生类中的函数不会覆盖基类中的虚函数，因为它们的参数列表不匹配。如果你希望在派生类中覆盖基类的
* 虚函数，你需要确保派生类中的函数具有与基类中的虚函数相同的参数列表，包括参数的类型和数量
*/
```



### 34：区分接口继承和实现继承

```C++
/* 公共继承的意思是“is-a”。虚函数意味着“接口必须继承”，而非虚函数意味着“接口和实现都必须继承” */

/* 看下面代码 */
class Shape { // 有一个 pure virtual 是抽象类
public:
    virtual void draw() const = 0; 			    // pure virtual
    virtual void error(const std::string& msg);	// impure virtual
    int objectID() const;				        // non-virtual
    ...
};
class Rectangle : public Shape { ... }; // 派生类
class Ellipse : public Shape { ... };   // 派生类

/*
* pure virtual：目的是让派生类只继承函数接口。可以为纯虚函数提供定义。但是调用它的唯一方法是用类名指定调用。
* impure virtual：目的是让派生类继承函数接口和默认实现。有时候，同时指定函数接口和默认实现是很危险的。
* non-virtual:
*/

Shape* ps = new Shape; 	// 错误! Shape 是抽象类
Shape* ps1 = new Rectangle; 	// 没问题
ps1->draw(); // Rectangle::draw
Shape* ps2 = new Ellipse; // 没问题
ps2->draw(); // Ellipse::draw
ps1->Shape::draw(); // Shape::draw
ps2->Shape::draw(); // Shape::draw


/* 看下面代码 */

class Airport { ... }; // 机场

class Airplane { // 一个关于飞机的基类
public:
    virtual void fly(const Airport& destination); // 飞行方式
    //...
};
void Airplane::fly(const Airport& destination){
    ...// 将飞机飞到给定目的地的默认代码
}

class ModelA : public Airplane { ... };  // A型飞机
class ModelB : public Airplane { ... };  // B型飞机

/* 
* A和B都是同样的方式飞行，并且使用的飞行方式是基类提供的默认,然而现在来了个C型，C同A和B有着全新的飞行方式
* 但是他们忘记了定义C的飞行方式，并且通过了编译，可以运行....这是一个灾难
*/

class ModelC : public Airplane {
    ... // 没有声明fly函数
}
Airport PDX(...); // PDX是一个机场
Airplane* pa = new ModelC;
...
pa->fly(PDX); // 调用 Airplane::fly!

/* 下面是一种解决方案 */

class Airplane { // 稍微改进一下基类
public:
    virtual void fly(const Airport& destination) = 0;// 只提供接口继承
    ...
protected:
    void defaultFly(const Airport& destination); // 如果需要使用默认飞行，需要显示调用这个新函数
};

class ModelA : public Airplane {
public:
    virtual void fly(const Airport& destination)
    {
        defaultFly(destination); // 明确的调用
    }
    ...
};


/* 我喜欢这种方法，并且也更好 */

class Airplane {
public:
    virtual void fly(const Airport& destination) = 0;
    ...
};
void Airplane::fly(const Airport& destination) // 一个纯虚函数的实现
{ 
    ...// 将飞机飞到给定目的地的默认代码
}

class ModelA : public Airplane {
public:
    virtual void fly(const Airport& destination)
    {
        Airplane::fly(destination); // 显式的调用父类的纯虚函数(飞行的默认实现)
    }
    ...
};

class ModelC : public Airplane {
public:
    // 除非显式的调用父类的pureVirtual,否则只是继承其接口
    virtual void fly(const Airport& destination); 
    ...
};
void ModelC::fly(const Airport& destination)
{
    ...// 将ModelC型飞机飞到给定目的地的特有代码
}

/* 
* 非虚函数：目的是让派生类强制继承基类的函数接口和实现。即，在派生类中不应该有不同的行为。
* 虽然编译器允许，但是我认为那只不过是变量的遮掩，并且可读性性差，既然有不同的实现，就声明为virtual
*/

```



### 35：考虑 virtual 函数以为的其他选择

```C++
/* 藉由非虚接口(non-virtual interface，NVI)实现 模板方法(Template Method)设计模式 */

/* 考虑下面一个计算人物血量的代码 */
class GameCharacter {
public:
    int healthValue() const // NVI，不允许派生类重新定义
    { 
        ... // 做一些“事前准备”工作（框架的一部分）
            int retVal = doHealthValue(); // 做真正的工作
        ... // 做一些“事后清理”工作（框架的一部分）
            return retVal;
    }
private:
    virtual int doHealthValue() const // 被隐藏在private里的虚函数，运行重写
    {
        ... // 计算角色生命值的默认算法
    } 
}

/* 派生类可以用重写private-non-virtual,尽管很怪,下面是个例子 */

class Derived: public GameCharacter{
public:
    //.... 
private:
    int doHealthValue() const; // 重写基类的private-non-virtual
}

// 使用基类指针
GameCharacter* g1 = new Derived();
g1->healthValue();// 调用子类的doHealthValue,如果是non-virtual则达不到这样的效果



/* 藉由函数指针(Function Pointer)实现策略(Strategy)设计模式 */

class GameCharacter; // 前置声明
int defaultHealthCalc(const GameCharacter& gc);// 计算健康状况的默认算法
class GameCharacter {
public:
    typedef int (*HealthCalcFunc)(const GameCharacter&); // 函数指针定义
    explicit GameCharacter(HealthCalcFunc hcf = defaultHealthCalc)// 提供一个默认方式
        : healthFunc(hcf)
    {}
    int healthValue() const
    {
        return healthFunc(*this);
    }
    ...
private:
    HealthCalcFunc healthFunc;// 函数指针
};

class EvilBadGuy : public GameCharacter {
public:
    explicit EvilBadGuy(HealthCalcFunc hcf = defaultHealthCalc)
        : GameCharacter(hcf)//调用基类构造方法
    {
        //...
    }
    //...
};
int loseHealthQuickly(const GameCharacter&); // 健康值计算函数1
int loseHealthSlowly(const GameCharacter&);  // 健康值计算函数2
EvilBadGuy ebg1(loseHealthQuickly); // 相同类型的实例
EvilBadGuy ebg2(loseHealthSlowly); // 具有不同的健康值计算你行为



/* 藉由 std::function 实现策略(Strategy)设计模式 */
/* 类似于函数指针,但是更灵活 */

class GameCharacter; // 和以前一样
int defaultHealthCalc(const GameCharacter& gc); // 和以前一样
class GameCharacter {
public:
    // HealthCalcFunc是“可调用的实体（callable entity）”
    // 它可以接受任何与GameCharacter兼容的东西，并返回任何与int兼容的东西
    typedef std::function<int(const GameCharacter&)> HealthCalcFunc;

    explicit GameCharacter(HealthCalcFunc hcf = defaultHealthCalc)
        : healthFunc(hcf)
    {}
    int healthValue() const
    {
        return healthFunc(*this);
    }
    ...
private:
    HealthCalcFunc healthFunc;
};

/* 下面的"函数"，是单单函数指针不能使用的 */
short calcHealth(const GameCharacter&); // 健康值计算函数，返回类型不是int
 
struct HealthCalculator { // 函数对象：计算健康值
    int operator()(const GameCharacter&) const // 甚至重载一个"()",就可以当作参数
    {
        ...
    } 
};
class GameLevel {
public:
    float health(const GameCharacter&) const; // 成员函数：计算健康值
    ... 
}; 

/* 人物对象 */
class EvilBadGuy : public GameCharacter { // 和以前一样
    ...
};
class EyeCandyCharacter : public GameCharacter { // 另一种人物类型
    ...  
}; 
 
EvilBadGuy ebg1(calcHealth);  
EyeCandyCharacter ecc1(HealthCalculator()); // 用了重载
GameLevel currentLevel;
...
EvilBadGuy ebg2( 
    std::bind( &GameLevel::health,  currentLevel, _1) 
);



/* 古典的策略(Strategy)设计模式 */
/* 为血量算法设计成一个新的类 */

class GameCharacter; // 前置声明
class HealthCalcFunc {
public:
    ...
    virtual int calc(const GameCharacter& gc) const
    {
        ...
    }
    ...
};
HealthCalcFunc defaultHealthCalc;


class GameCharacter {
public:
    explicit GameCharacter(HealthCalcFunc* phcf = &defaultHealthCalc)
        : pHealthCalc(phcf)
    {}
    int healthValue() const
    {
        //     基类指针，通过虚函数调用正确的函数，传入*this计算血量
        return pHealthCalc->calc(*this);
    }
    ...
private:
    HealthCalcFunc* pHealthCalc; // 指向继承自HealthCalcFunc派生类对象
};

/*
* virtual函数的替代方案包括NVI和策略设计模式的多种形式。NVI本身是一种特殊形式的Template Method设计模式。
* 将功能从成员函数移到class外部，带来一个缺点，非成员函数无法访问class的非public成员
* function对象的行为就行一般函数指针。这样的对象可接纳“满足目标签名式”的所有可调用实体。
*/

```



### 36：绝不重新定义继承而来的 non-virtual 函数

````C++
/* 细看下面代码 */

class B {
public:
    void mf(); // non-virtual函数
    ...
};
class D : public B { ... }; // 派生类D中没有重新定义基类的 non-virtual mf

D x; 		
B* pB = &x;	
pB->mf();  // 没错调用 B::mf 	

D* pD = &x;	
pD->mf();// 在D中查找名为 mf的函数没有找到，向基类中查找找到了，调用B::mf

/* 值得注意的是mf的调用是根据指针类型来调用的尽管会向基类中查找，但是动作是不一样的，静态绑定(编译时确定的) */

/* 如果D中重写了基类的 non-virtual mf */
class D : public B {
public:
    void mf();
};

D x; 		
B* pB = &x;	
pB->mf();  // 调用 B::mf ，与多态背道而驰	

D* pD = &x;	
pD->mf(); // 调用 D::mf

/* 
* 虽然编译没有问题，但是我认为逻辑漏洞非常大，如果重写就需要声明为virtual,如果就是实现上面的需求(奇怪)
* 那你就应该有一个新的函数，而不是重写一个 non-virtual
*/

````



### 37：绝不重新定义继承而来的缺省参数值

````C++
/* 首先该函数一定是虚函数(见36) */

/* 直入正题，下面是一个图形类，并且可以指顶draw的颜色 */


class Shape { // 有一个 pure virtual ,是抽象类无法实例化
public:
    enum ShapeColor { Red, Green, Blue }; // 颜色枚举
     // 所有形状都必须提供一个函数来绘制自己
    virtual void draw(ShapeColor color = Red) const = 0;

};

class Rectangle : public Shape {
public:
    // 注意到不同的默认参数值了吗? 情况不妙!
    virtual void draw(ShapeColor color = Green) const{
        std::cout << "Drawing a rectangle with color " << color << std::endl;
    }
};


class Circle : public Shape {
public:
    // 没有默认参数
    virtual void draw(ShapeColor color) const{
        std::cout << "Drawing a Circle with color " << color <<std::endl;
    }
};


int main(){
    
    Shape* pCir = new Circle; 	// 静态类型 = Shape*
    Shape* pRect = new Rectangle; 	// 静态类型 = Shape*
    Circle* r1 = new Circle;

    pCir->draw(); // 虽然派生类中没有指定默认参数，但是使用的是基类的默认参数
    pRect->draw(); // 默认参数不是ShapeColor::Green，而是基类的默认参数ShapeColoe::Red
    r1->draw(); // 缺少参数，编译失败
    return 0;
}

/* 
* 默认参数竟然是静态绑定，尽管函数是virtual，但是函数仍然是静态绑定，这就出现了一个很奇怪的现象
* 函数调用的是派生类，而默认参数使用的是基类。
*/

/* 
* 一种坏的解决方法，那就把基类和派生类的默认都设置成一样的，就不会有歧义了，但是如果基类的默认改成其他颜色的
* 派生类的默认还得一个一个改，代码依赖性大，下面就是这种坏注意的实现
*/

class Shape {
public:
    enum ShapeColor { Red, Green, Blue };
    virtual void draw(ShapeColor color = Red) const = 0;
    ...
};
class Rectangle : public Shape {
public:
    virtual void draw(ShapeColor color = Red) const;
    ...
}



/* 
* 有坏主意就有好主意，就直接将基类的draw函数写成 non-virtual，然后默认为red,然后真正的draw真正实现的部分
* 交给另一个函数，下面是实现
*/

class Shape {
public:
    enum ShapeColor { Red, Green, Blue };
    //现在是non-virtual，派生类如果调用draw一定是这一个(如果你遵守36)
    void draw(ShapeColor color = Red) const 
    {
        doDraw(color); // 调用一个 virtual
    }
    ...
private:
    // 真正干活的地方，写成private是合理的(见35的NVI设计模式)
    virtual void doDraw(ShapeColor color) const = 0; 
}; 
class Rectangle : public Shape {
public:
    ...
private:
    virtual void doDraw(ShapeColor color) const; // 注意没有默认值
    ... 
};

````



### 38：通过复合塑膜出 has-a 或 “根据某物实现出”

````C++
/* 什么是 has-a 关系或者说 根据某物实现出，请看下面代码 */

class Address { ... }; // 某人住址
class PhoneNumber { ... };

class Person {
public:
    ...
private:
    std::string name; 	     // 合成成分物,has-a关系
    Address address; 		 // 同上
    PhoneNumber voiceNumber; // 同上
    PhoneNumber faxNumber; 	 // 同上
};

/* 这很好理解，考虑下面的情景 */

/* 
* 假设需要创建一个容器类型，存放的元素不可以重复，一般 std::set 就可以了，但是现在的情景是
* 希望节省空间，对速度的需求较小，所以利用标准库的 list template 是一个不错的选择，考虑下面实现
*/

template<typename T> // 将list用于Set的错误方法
class Set : public std::list<T> { ... };

/* 
* 可以看出上面是用了继承，但是逻辑上有了错误，set 是不允许重复的，但是list可以，还记得吗，
* public 是is-a的关系，显然set并不是一个list，我们应该使用has-a的关系，或者说 set根据list实现出！
*/

template<class T> // 将list用于Set的正确方法
class Set {
public:
    // 下面的函数可以利用 list 的接口模仿出来 set 的行为
    bool member(const T& item) const; // 查找元素
    void insert(const T& item); // 插入
    void remove(const T& item); // 移除
    std::size_t size() const;
private:
    std::list<T> rep; // Set数据的表示，“is-implemented-in-terms-of”的关系
};

template<typename T>
bool Set<T>::member(const T& item) const{
    return std::find(rep.begin(), rep.end(), item) != rep.end();
}

template<typename T>
// 插入元素时判断是否已经存在该元素
void Set<T>::insert(const T& item){ if (!member(item)) rep.push_back(item);}

template<typename T>
void Set<T>::remove(const T& item){
    typename std::list<T>::iterator it = std::find(rep.begin(), rep.end(), item); 
    if (it != rep.end()) rep.erase(it);
}

template<typename T>
std::size_t Set<T>::size() const{ return rep.size();}

````



### 39：明确而审慎地使用 private 继承

````C++
/* 
* 首先需要理解 private 的特征，就是无论基类中的成员函数和变量是public或protected ,一个私有继承它的派生类
* 都会使其在该派生类中成为private(类似于派生类自己的private,在类的内部可以调用，基类的private，
* 在派生类中仍然不可直接访问)，然后就是编译器通常不会把一个派生类转换成基类，例如下面代码
*/

class Person { ... };
class Student : private Person { ... }; // private继承

void eat(const Person& p); 		// 只要是Person，就可以eat
void study(const Student& s); 	// 只有Student才能study

Person p; 	
Student s; 	
eat(p); // 没问题
eat(s); // 错误!因为是私有继承，所以编译器不会自动的向基类转换

/* 
* private 继承应该是is-implemented-in-terms-of(根据某物实现出),同复合很相似，
* 因此如果能用复合代替private ，就尽量，但是如果private更好，我们也应该使用它
*/

/* 假如我们有一个Widget Class ,现在我想直到它的调用情况，我们找到了一个 Timer Class,我们怎么使用它? */

class Timer {
public:
    explicit Timer(int tickFrequency);
    virtual void onTick() const; // 定时器每次到时间，就调用一次该函数
    ...
};

/* 如果使用Widget public 继承 Timer 显然是错误的，那private 呢 */

class Widget : private Timer {
private:
    // 声明为 private,防止客户误用,并且有更好的封装(见18)
    virtual void onTick() const; // 查看Widget使用的数据等。
    ...
};

/* 没有什么错误，但是可以用别的方法代替它 */
/* 
* 这里的私有继承并不是必须的。也可以使用复合。只需在Widget内部声明一个私有的嵌套类，
* 它将公开继承自Timer，在那里重新定义onTick，并将该类型的对象放在Widget内部
*/
class Widget {
private:
    class WidgetTimer : public Timer {
    public:
        virtual void onTick() const;
        ...
    };
    WidgetTimer timer;
    ...
};


/* 在极端的情况下，private 可以节省空间，即 空基优化(EBO) */

class Empty {}; // 没有数据，应该不占内存
 
class HoldsAnInt { // 应该只占一个int
private:
    int x;
    Empty e; // 实际上编译器会安插最少一个字节
};


/* 空基优化，但是随着时间的推移，C++20说不好可能已经有新的实现了 */
class HoldsAnInt : private Empty {
private:
    int x;
};

````



### 40：明智而审慎的使用多重继承

```C++
/* 多重继承有时候很合理，有时候也会有一些困扰，看下面代码 */

class Base_A{
public:
    void func_A(int x); // public 
};

class Base_B{
private:
    void func_A(int x); // private
}

class Derived: public Base_A, public Base_B{ // 同时继承Base_A and Base_B
	//...
};

Derived* d1 = new Derived();
d1->func_A(); // 产生歧义，调用的是Base_A中的还是Base_B中的

/* 
* 可以知道的是Base_B中的func_A函数是private,所以派生类是无法调用的，所以d1是调用的Base_A中的？
* 不对，C++首先确定确定谁是最匹配的函数。它只在找到最佳匹配的函数后才检查可访问性。因此执行出错
*/

// 多继承尽量如此调用，避免产生歧义
d1->Base_A::func_A;


/* 多继承引发的另一个问题“钻石型多继承” */


class File { ... };

class InputFile : public File { ... };
class OutputFile : public File { ... };

class IOFile : public InputFile,
    public OutputFile
{
    ...
};
/* 设想一下File中的FileName成员变量是以什么样的姿态呈现在 IOFile */

/*
* 是否打算让File内的成员经由两条不同的路径被复制到IOFile？如果不想，应当使用虚继承，表面共享基类：
*/

class File { ... };  // 不变

class InputFile : virtual public File { ... }; // 虚继承
class OutputFile : virtual public File { ... };// 虚继承

class IOFile : public InputFile,  // 不变
    public OutputFile
{
    ...
};

/* 
* 虚继承通常占用更多空间，速度也受影响,所以谨慎使用多继承
* 难道多继承就这么不堪吗，下面这种情形可能是令多继承大展身手
*/


class IPerson{ // 从下面的函数可以看出IPerson是一个抽象类，一个可以作物基类的类
public:
    virtual ~IPerson(){}
    virtual std::string name() const = 0; // 注意 pure virtual
    //...
}

class PersonInfo {
public:
    explicit PersonInfo(DatabaseID pid); // 需要一个数据库ID,通过这个ID可以查看姓名等信息
    virtual ~PersonInfo();
    virtual const char* theName() const;  // 利用ID查看姓名
    virtual const char* theBirthDate() const; // 利用ID查看生日
    ...
private:
    // 下面是一种前后缀格式化输出形式例如调用上面的theName,会返回 “[Tom]”
    virtual const char* valueDelimOpen() const; // 返回“[”
    virtual const char* valueDelimClose() const; // 返回“]”
    ...
};

/* 
* 现在有一个新的类CPerson需要IPerson的接口，也想要PersonInfo的实现 ，一种方法是public继承IPerson，然后
* 在CPerson内置一个 PersonInfoImpl指针，但是客户有一个需求，希望可以设置格式化输出的前后缀，
* 由此引出多继承，CPerson public 继承 IPerson 其接口,private 继承 PersonInfo其实现
*/

class CPerson : public IPerson, private PersonInfo { // 注意：多重继承
public: // 接口
    explicit CPerson(DatabaseID pid) : PersonInfo(pid) {}
    virtual std::string name() const 
    {
        return PersonInfo::theName();
    } 

    virtual std::string birthDate() const 
    {
        return PersonInfo::theBirthDate();
    }
private: // 重新定义分隔符虚函数的实现 // 重写父类的 private虚函数这是合理的(见35)
    const char* valueDelimOpen() const { return ""; }  
    const char* valueDelimClose() const { return ""; } 
}; 

/*
* 多重继承比单继承复杂。它可能会导致新的歧义问题和对虚继承的需求。
* 虚拟继承会在大小、速度以及初始化和赋值的复杂性方面增加开销。当虚拟基类没有数据时，它是最实用的。
* 多重继承确实有合理的用途。一种情况是将接口类的公共继承与有助于实现的类的私有继承相结合。
*/

```





## 七、模板与泛型编程



### 41：了解隐式接口和编译器多态

````C++
/* C++面向对象部分总是以显示接口和运行期多态来解决问题，比如下面 */

class Widget { // 一个平平无奇的类
public:
    Widget();
    virtual ~Widget();
    virtual std::size_t size() const; // virtual函数，拥有运行期多态
    virtual void normalize();
    void swap(Widget& other); 
    ...
}

void doProcessing(Widget& w) // w类型是Widget,这是显式接口
{
    if (w.size() > 10 && w != someNastyWidget) {
        Widget temp(w);
        temp.normalize();
        temp.swap(w);
    }
}


/* 进入模板和泛式编程的世界，主场就变为了隐式接口和编译期多态(显示接口和运行期多态仍然存在) */
/* 改写上面的doProcessing函数 */

template<typename T>
void doProcessing(T& w) // w是不确定的，但是根据下面的表达式可知它必须满足下面的表达式
{
    if (w.size() > 10 && w != someNastyWidget) {
        T temp(w);
        temp.normalize();
        temp.swap(w);
    }
}

/*
* w必须支持的接口由模板中对w执行的操作决定(隐式)
* 因为使用不同的模板参数实例化函数模板会导致调用不同的函数，这被称为编译时多态性
*
* 类和模板都支持接口和多态性。
* 对于类来说，接口是显式的，并且以函数签名为中心。多态性通过虚函数在运行时发生。
* 对于模板参数，接口是隐式的，并且基于有效的表达式。多态性在编译过程中通过模板实例化和函数重载解析发生与编译期。
*/

````



### 42：了解 typename 的双重含义

````C++
/* 下面声明template 参数的形式是等价的(typename 的第一种用法，很简单) */

template<class T> class Widget; 	// 使用关键词 "class"
template<typename T> class Widget; 	// 使用关键词 "typename"


/* 为了引入 typename的另一种含义，看下面代码 */

template<typename C> 
void print2nd(const C& container) // 打印容器中的第二个元素(一个没事找事的类，别在意)
{ // 这不是有效的C++代码!!!!!!!!!!!!!!!!!!!!!!
    if (container.size() >= 2) {
        C::const_iterator iter(container.begin()); // 取得第一个元素的迭代器
        ++iter; 					// 将iter移动到第二元素
        int value = *iter; 				// 将该元素拷贝到一个int变量
        std::cout << value; 				// 打印该int值
    }
}

/*
* 值得注意的是C::const_iterator是一个嵌套的从属名称，它的迭代器类型是取决于C的类型的
* 这就引出了一个问题，嵌套从属名可能导致解析困难
*/

template<typename C>
void print2nd(const C& container)
{
    /* 如果C有一个名为const_iterator的静态数据成员，如果x是一个全局变量的名称，会怎么样? 
    C::const_iterator 乘以 x,尽管这很疯狂，但是不是不可能
    并且默认情况下C++认为嵌套的从属名称不是类型。所以上面的并不是有效的C++代码
    */
    C::const_iterator* x;
    ...
}


/* 因此我们必须告诉C++它是一个类型，这就引出了 typename的第二种用法 */
template<typename C> // 这是有效的C++代码
void print2nd(const C& container)
{
    if (container.size() >= 2) {
        // 在从属嵌套名称前加上 typrname ，告诉编译器它是一个类型
        typename C::const_iterator iter(container.begin());
        ...
    }
}



/* typename只能用于标识嵌套的从属类型名称 */
template<typename C> 			// 允许使用typename(就像"class"一样)
void f(const C& container, 		// 不可以使用typename
    typename C::iterator iter); 	// 必须使用typename 





/* 
* 但是并不是所有从属嵌套名称(前提是它应该是个类型)前加上 typrname 
* typename不能放在基类列表中的嵌套从属类型名称之前，也不能放在成员初始化列表中的基类标识符之前。
*/

template<typename T>
class Derived : public Base<T>::Nested { // 基类列表: 不能使用typename 
public: 
    explicit Derived(int x)
       : Base<T>::Nested(x) // 初始化列表中：不能使用typename 
    { 
        typename Base<T>::Nested temp; // 其他情况下，需要使用typename
        ... 				   //指明是嵌套从属类型名称
    } 
    ... 
};



/* 假设我们正在编写一个接受迭代器的函数模板，并希望创建迭代器指向的对象的本地副本temp */

template<typename IterT>
void workWithIterator(IterT iter)
{
//将 std::iterator_traits<IterT>::value_type类型用typename指定，然后利用typedef起一个别value_type
    // 详细了解std::iterator_traits<IterT>::value_type 见47
    typedef typename std::iterator_traits<IterT>::value_type value_type;
    value_type temp(*iter);
    ...
}

````



### 43：学习处理模板化基类内的名称

````C++
/* 如果现在需要一个可以向不同工资发送信息的类 */

class CompanyA {  // 公司类
public:
    ...
    void sendCleartext(const std::string& msg); // 明文发送
    void sendEncrypted(const std::string& msg); // 加密发送
    ...
};
class CompanyB {  // 公司类
public:
    ...
    void sendCleartext(const std::string& msg); // 明文发送
    void sendEncrypted(const std::string& msg); // 加密发送
    ...
};

// ....还有其他的公司

class MsgInfo { ... }; // 用于保存信息的类，将来可以用生成消息


template<typename Company> 
class MsgSender {  // 模板类
public:
    ... // ctors, dtor, 等等.
    void sendClear(const MsgInfo& info) 
    {
        std::string msg; // 用来从 info 中拿到信息
        ...// 通过info创建消息;
        Company c;
        c.sendCleartext(msg); // 调用这个公司的方法发送明文
    }
    void sendSecret(const MsgInfo& info) // 
    {
        ... // 与sendClear类似，但会调用c.sendEncrypted发送密文
    } 
};


/* 假设我们想在发送信息的时候记录一些信息，达到日志得效果，我们又创建了一个LoggingMsgSender模板类 */
template<typename Company>
class LoggingMsgSender : public MsgSender<Company> {
public:
    ... // ctors, dtor, 等.
    void sendClearMsg(const MsgInfo& info){
        ...//在日志中写入“发送前"信息;
        sendClear(info); // 调用基类函数;无法编译通过！！
        ...//在日志中写入“发送后"信息;
    }
    ...
};


/* 
* sendClear(info) 会报错，提示没有这个函数，它们不是继承关心吗，无论是继承还是函数声明都是pupblic的，
* 这就引出了一个问题，我们现把这个问题搁置，现在有了一个公司Z
*/


class CompanyZ { // 因为这个公司坚持使用密文发送，所以这个类压根没有提供sendCleartext函数
public: 
 ...
 void sendEncrypted(const std::string& msg);
 ...
};


/* 但是MsgSender类中sendClear接口是明明白白需要这个接口的，因此需要特殊处理(全特化) */

//基类模板可以进行特化，而且这种特化可能不会提供与通用模板相同的接口
template<> // 全特化，如果company是 CompanyZ就会使用这个版本
class MsgSender<CompanyZ> { // 没有sendCleartext，其他都和泛化的版本一样
public: 
    ... 
    void sendSecret(const MsgInfo& info)
    {
        ...
    }
};

/* 
* 也就是MsgSender 的派生类是知道如果使用的是基类特化的版本可能会出错，比如使用CompanyZ，
* 因此我们要想编译器承诺，这个基类是有这个函数的(找不到依然会编译错误哈)
*/

/* 解决方法 1：加上一个this,向编译器承诺，一定有有个这样的接口在基类或该类(无论是否被特化) */
template<typename Company>
class LoggingMsgSender : public MsgSender<Company> {
public:
    ...
    void sendClearMsg(const MsgInfo& info)
    {
        ...//在日志中写入“发送前"信息;
        this->sendClear(info); // OK！假设sendClear将被继承
        ...//在日志中写入“发送后"信息;
    }
    ...
};



/* 解决方法2： 使用using 声明*/
template<typename Company>
class LoggingMsgSender : public MsgSender<Company> {
public:
    using MsgSender<Company>::sendClear; // 告诉编译器，请它假设基类里有sendClear
    ... 
    void sendClearMsg(const MsgInfo& info)
    {
        ..
        sendClear(info); // OK！！假设sendClear将被继承
        ... 
    }
    ...
};


/* 解决方法3：要明确指定被调用的函数在基类中有定义 */

template<typename Company>
class LoggingMsgSender : public MsgSender<Company> {
public:
    ...
    void sendClearMsg(const MsgInfo& info)
    {
        ...
        // 直接写死了，破坏的多态性，万一是virtual呢
        MsgSender<Company>::sendClear(info); // OK！！假设sendClear将被继承
        ... 
    } 
    ...
};

/* 
* 这些方法都做了同样的事情：向编译器承诺，基类模板的任何后续特化都将支持通用模板提供的这些接口。
* 如果违反了承诺，编译将会失败：
*/

/* 下面是我模仿上面代码写的一个旨在于清晰的说明该问题，而无实际意义的实现 */


class companyA{ // 简化的类的实现
public:
    void func_1(){std::cout<<"companyA__func_1\n";}

    void func_2(){std::cout<<"companyA__func_2\n";}
};

class companyB{
public:
    void func_1(){std::cout<<"companyB__func_1\n";}

    void func_2(){std::cout<<"companyB__func_2\n";}
};


class companyZ{
public:
    void func_1(){std::cout<<"companyZ__func_1\n";}

    // 同其他公司不同 ，它没有 func_2 接口
};




template<typename company>
class Base{ 
public:
    void base_func_1(){
        company c;
        c.func_1(); // 调用 company 的接口
    }

    void base_func_2(){
        company c;
        c.func_2();
    }
};


template<typename company>
class Derived: public Base<company>{
public:
    void derived_func_1(){
        this->base_func_1(); // 调用父类的方法，用了this指针，向编译器承诺
    }
    void derived_func_2(){
        this->base_func_2(); // 同上
    }
};


template<>
class Base<companyZ>{  // 特化 Base 对 companyZ 的实现 
public:
    void base_func_1(){
        companyZ c;
        c.func_1();
    }
};




int main(){
    Derived<companyA>* d1 = new Derived<companyA>;
    d1->derived_func_1();
    d1->derived_func_1();
    Derived<companyZ>* d2 = new Derived<companyZ>;
    d2->derived_func_1();
    d2->derived_func_2(); // 有问题，companyZ 无法调用该接口
    return 0;
}

````



### 44：将于参数无关的代码抽离 template

````C++
/* 记住，class templates 的成员函数只有在被使用的时候才会被暗中具现化 */
/* 下面是一个关于矩阵的模板类 */

template<typename T, std::size_t n> // nxn个T类型对象矩阵的模板，n为非类型参数
class SquareMatrix { 
public:
    ...
    void invert(); // 对矩阵求逆
};

/* 下面是对该类的调用 */
SquareMatrix<double, 5> sm1;
sm1.invert(); // 调用 SquareMatrix<double, 5>::invert
SquareMatrix<double, 10> sm2;
sm2.invert(); // 调用 SquareMatrix<double, 10>::invert

/* 
* 我们发现虽然数据类型都是double，但是矩阵的n只要变化就会产生一种新的类型，那如果有多个不同的矩阵，
* 那目标码岂不是很庞大，可不可把n当成一个参数那，不作为类型的判断依据，可能是不行的，如果矩阵之间要做运算呢，
* 比如加法运算，不同的n直接是没有办法做加法的，所以n是必要的，但是不同类型的矩阵做加法运算的过程是一样的，
* 做逆运算也是，因此可以考虑，把这一块给抽取出来，下面是一个实践
*/


template<typename T> // 与大小无关的方阵基类
class SquareMatrixBase { 
protected:
    //...
    void invert(std::size_t matrixSize); // 求给定大小的逆矩阵
    //...
};

template< typename T, std::size_t n>
class SquareMatrix : private SquareMatrixBase<T> {
private:
    using SquareMatrixBase<T>::invert; // 避免隐藏invert的基类版本
public:
    //...
    void invert() { this->invert(n); } // 内联调用基类版本的invert
}; 

/*
* 这里我要提一嘴，使用using 是防止Base::invert被Derived::invert()遮盖，这是必须的，尽管你将基类的invert
* 设置成virtual,因为它们的参数列表不一样，它们只能是重载关系(见33)
* 派生类中调用Base::invert使用的了this指针，当然你也可以直接使用Base::invert指定(具体见43)
*/ 

/*
* 回到正题，SquareMatrixBase中定义了 invert,看看它的声明template<typename T>，也就是说，无论这个
* 矩阵的n是多少，只要数据类型一样(比如double,int),它就只有一个版本的逆运算
*/


/* 
* 好了，似乎完事大吉了，如果需要其他运算，往SquareMatrixBase里面加就是了，
* 但它如何知道特定矩阵的数据在哪里呢?如果还是通过函数的参数传入指向数据的指针，
* 将可能需要在不同的函数里重复传入相同的指针。所以我们决定把数据的（连同大小）都存在基类
* (其实我感觉和这小节关系不大)
*/

template<typename T>
class SquareMatrixBase {
protected:
    SquareMatrixBase(std::size_t n, T* pMem) //存储矩阵大小和数据的指针
        : size(n), pData(pMem) {} 
    void setDataPtr(T* ptr) { pData = ptr; } // 为pData赋值
    ...
private:
    std::size_t size; 	// 矩阵大小
    T* pData; 			// 指向矩阵数据的指针
};

template<typename T, std::size_t n>
class SquareMatrix : private SquareMatrixBase<T> {
public:
    SquareMatrix()          
	: SquareMatrixBase<T>(n, data) // 将矩阵大小和数据的指针传送给基类
    {} 
    ...
private:
    T data[n * n];
};



/* 或者将每个矩阵的数据放到堆中 */

template<typename T, std::size_t n>
class SquareMatrix : private SquareMatrixBase<T> {
public:
    SquareMatrix() 
        : SquareMatrixBase<T>(n, 0), // 设置基类数据指针为空
        pData(new T[n * n]) // 为矩阵值分配内存，并保存其指针
    {
        this->setDataPtr(pData.get()); //在基类保存一份指针的副本
    }  
    ... // 
private:
    std::shared_ptr<T> pData; 
}; 

````



### 45：运用成员函数模板接受所有兼容类型

````C++
/* 众所周知普通指针在类的继承体系中是支持隐式转换的，看下列代码 */

class Top { ... };
class Middle : public Top { ... };
class Bottom : public Middle { ... };

Top* pt1 = new Middle; //  Middle* 转换为 Top*
Top* pt2 = new Bottom; //  Bottom* 转换为 Top*
const Top* pct2 = pt1; //  Top*  const Top*


/* 现在我想用指针指针，但是我又希望它们支持隐式转化(C++ 内置智能指针不提供该功能),因此需要自己动手 */

template<typename T>
class SmartPtr {
public:  
    explicit SmartPtr(T* realPtr); // 智能指针通常由内置指针初始化
    ...
};

/* 这是我们的期望，但是靠上面的代码是不可能的这是不可能的 */

SmartPtr<Top> pt1 = SmartPtr<Middle>(new Middle); // SmartPtr<Middle> 转换为 SmartPtr<Top>

SmartPtr<Top> pt2 = SmartPtr<Bottom>(new Bottom); // SmartPtr<Bottom> 转换为 SmartPtr<Top>

SmartPtr<const Top> pct2 = pt1;  // SmartPtr<Top> 转换为 SmartPtr<const Top>

/* 如果一个一个的写又不太现实，我们可以将构造函数也模板化 */

template<typename T>
class SmartPtr {
public:
    template<typename U> // “构造函数”模板，成员函数模板
    SmartPtr(const SmartPtr<U>& other); // 注意：是non-explicit 
    ...  
};


/* 其实这样看还是又点迷糊(至少我是这样)，这个模板构造函数里面写什么呢？如果是父类向子类的转化应该怎么控制呢? */

template<typename T>
class SmartPtr {
public:
    template<typename U>
    SmartPtr(const SmartPtr<U>& other) //用其他对象的保存指针初始化这个对象 
        : heldPtr(other.get()) {
        ...
    }  
    T* get() const { return heldPtr; }
    ...
private:  
    T* heldPtr; // SmartPtr持有的内置指针
};

/* heldPtr(other.get()),这个代码调用了原始指针的隐式转换规则，如果可以怎通过否则不通过 */


/*
* 需要注意的是声明泛化的拷贝构造函数(成员模板)不会阻止编译器生成自己的拷贝构造函数(非模板)，
* 想控制拷贝构造的方方面面，必须同时声明泛化和“普通”版本的拷贝构造函数。赋值也一样。
*/

template<class T> class shared_ptr {
public:
    shared_ptr(shared_ptr const& r); // 拷贝构造函数 
    template<class Y>  
    shared_ptr(shared_ptr<Y> const& r); // 泛化的拷贝构造函数 
    shared_ptr& operator=(shared_ptr const& r); // 
    template<class Y> 
    shared_ptr& operator=(shared_ptr<Y> const& r); // 泛化的拷贝赋值
    ...
};

````



### 46：需要类型转换时请为模板定义非成员函数

````C++
/* 
* 24中讨论了为什么 non-member函数 才有能力在所有实参上实施隐式转换(Rational 的 operator*)，如今将
* template Rational 又会如何呢
*/

template<typename T>
class Rational { // template Rational
public:
    Rational(const T& numerator = 0,  const T& denominator = 1); 
    const T numerator() const; 
    const T denominator() const; 
    ... 
};

// 非成员函数的 operator* 但是时 template
template<typename T>
const Rational<T> operator*(const Rational<T>& lhs,const Rational<T>& rhs)
{
    ...
}

/* 按照我们的期望下面的代码都是可以调用成功的 */

Rational<int> r1(1,2);
Rational<int> r2 = r1 * 2; // 编译错误

/* 
* 还记得条款1吗，在泛式编程领域，有时候面向对象的特点可能不太一样，
* 导致错误的原因是，调用 r1 * 2 ,编译器还没有具现化 
* Rational<int> operator*(const Rational<int>& lhs,const Rational<int>& rhs) 这个函数，
* 并且它是不会出现这种操作 const Rational<T>& rhs 隐式转换 int,
* 也就是说template 实参推导过程中不会将隐式类型转换纳入考虑，怎么办呢，一个可行的方法，就是在r1具现化的
* 时候就将operator具现化出来，所以将operator*声明为友元(member函数是用到的时候才具现化，况且声明为member是不* 合适的)
*/

template<typename T>
class Rational {
public:
    ...
    /* 当对象r1被声明为Rational<int>类型时，Rational<int>类被实例化，并且作为这个过程的一部分，接收Rational<int>参数的友元函数运算符*被自动声明。*/

    friend 
    const Rational operator*(const Rational& lhs, const Rational& rhs); 
};

template<typename T> // 定义
const Rational<T> operator*(const Rational<T>& lhs, const Rational<T>& rhs)
{
    ...
}

/* 
* ok,编译通过了，但是连接错误，所以其实在上面我们只给了一个声明??,是的下面定义的代码与上面声明没有关系，
* 这确实令人吃惊，一个简单的方式就是直接将定义合并到声明中，就像这样
*/

template<typename T>
class Rational {
public:
    ...
   friend const Rational operator*(const Rational& lhs, const Rational& rhs){
        return Rational(lhs.numerator() * rhs.numerator(),
            lhs.denominator() * rhs.denominator());   
    }   
};

/* 下面是完整代码 */




template<typename T>
class Rational { // template Rational
public:
    Rational(const T& numerator = 9,  const T& denominator = 8); 
    const T getNumerator() const{return numerator;}
    const T getDenominator() const{return denominator;}
    void showAll(){std::cout<<numerator<<" "<<denominator<<std::endl;}

    /* 如果该友元函数比较长的话，将其作为一个隐式inline不太好(编译器会优化)，我们可以用另一个函数转接一下
       放心功能依旧可以实现*/
    friend const Rational operator*(const Rational& lhs,const Rational& rhs){
        return doSomething(lhs,rhs);
    }
private:
    T numerator;
    T denominator;
};

template<typename T> // 构造
Rational<T>::Rational(const T& numerator, const T& denominator):
    numerator(numerator),
    denominator(denominator)
{}


template<typename T> // 转接函数
const Rational<T> doSomething(const Rational<T>& lhs,const Rational<T>& rhs)
{
    return Rational<T>(lhs.getNumerator() * rhs.getNumerator(), lhs.getDenominator() * rhs.getDenominator());
}

````



### 47：请使用 traits classes 表现类型信息

````C++
/* 思考如何实现下面功能 */

template<typename IterT, typename DistT>  
void advance(IterT& iter, DistT d); //前进iter d个单位;如果d<0，将iter向后移动 

/* 迭代器有很多类型，所以实现并不是iter+=d这么简单，需要考虑不同的迭代器类型 */

struct input_iterator_tag {}; // 只可以向前移动，一次一步，只可读一次
struct output_iterator_tag {}; // 只可以向前移动，一次一步，只可写一次
struct forward_iterator_tag : public input_iterator_tag {}; // 只可以向前移动，一次一步，读或写一次
// 继承自 forward ,但是可以向后移动
struct bidirectional_iterator_tag : public forward_iterator_tag {};
// 继承自 bidirectional ,但是一次可以跨越多个
struct random_access_iterator_tag : public bidirectional_iterator_tag {};


/* 下列是另一种实现 */
// 这就是traits的作用:它们允许你在编译期间获取有关类型的信息。
template<typename IterT, typename DistT>
void advance(IterT& iter, DistT d){
    if (iter is random_access_iterator_tag) {
        iter += d;  
    }
    else {
        if (d >= 0) { while (d--) ++iter; }  
        else { while (d++) --iter; }  
    } 
}


/*
* trait不是C++中的关键字或预定义结构；它们是C++程序员遵循的一种技术和约定。对该技术的要求之一是，
* 它必须像对自定义类型一样对待内置类型。意味着在类型中嵌套信息之类的无法实现，因为没有办法在指针内部嵌套信息。
* 任何自定义的迭代器类型都必须包含一个嵌套的名为iterator_category的typedef，用于确认迭代器的分类。
*/


template<typename IterT> 
struct iterator_traits; // 关于迭代器类型信息的模板


deque迭代器的类如下所示:
template < ... > // 模板参数被省略
class deque {
public:
    class iterator {
    public:     
        typedef random_access_iterator_tag iterator_category;
        ...
    } :
    ...
};

template<typename IterT>
struct iterator_traits {
    typedef typename IterT::iterator_category iterator_category;
    ...
}; // 这对于自定义类型很有效，但对于指针类型的迭代器根本不起作用，因为不存在具有嵌套typedef的指针



template<typename IterT> 	 
struct iterator_traits<IterT*> // 内置指针类型的模板偏特化
{
    typedef random_access_iterator_tag iterator_category;
    ...
};


template<typename IterT, typename DistT>
void advance(IterT& iter, DistT d)
{
    if (typeid(typename std::iterator_traits<IterT>::iterator_category) ==
        typeid(std::random_access_iterator_tag))
        ...
} // 只可惜typeid运行时获知变量类型名称,我们希望在编译时获取


/* 一种巧妙的方式就是利用重载 */

template<typename IterT, typename DistT> // 用于随机访问迭代器
void doAdvance(IterT& iter, DistT d, std::random_access_iterator_tag)  
{
    iter += d;
}

template<typename IterT, typename DistT> // 用于双向迭代器
void doAdvance(IterT& iter, DistT d, std::bidirectional_iterator_tag) 
{
    if (d >= 0) { while (d--) ++iter; }
    else { while (d++) --iter; }
}

template<typename IterT, typename DistT> // 用于输入迭代器
void doAdvance(IterT& iter, DistT d, std::input_iterator_tag)
{
    if (d < 0) {throw std::out_of_range("Negative distance"); }
    while (d--) ++iter;
}

template<typename IterT, typename DistT>
void advance(IterT& iter, DistT d)
{
 doAdvance(iter, d, typename std::iterator_traits<IterT>::iterator_category()); 
} 

/* tip: 一知半解 */

````



### 48： 认识 template 元编程

````C++
/* 
* 模板元编程(Template metadata programming, TMP)是编写在编译期间执行的基于模板的C++程序的过程。
* TMP是被发现的，而不是发明的。
*/

/* 
* TMP已经被证明是图灵完备的，这意味着它足够强大，可以计算任何东西。
* TMP使用递归模板实例化来代替循环,比如计算阶乘
*/

template<unsigned n>
struct Factorial{
    enum {value = n * Factorial<n-1>::value};
};

template<>
struct Factorial<0>{
    enum {value = 1};
};

std::cout<<Factorial<5>::value<<std::endl;



/*
* 模板元编程可以将工作从运行时转移到编译时，从而支持更早的错误检测和更高的运行时性能。
* TMP可用于根据策略选择的组合生成自定义代码，它还可用于避免生成不适合特定类型的代码。
*/

````





## 八、定制 new 和 delete



### 49：了解 new-handler 的行为

````C++
/* 
* 所谓 new-handler 就是如果new失败了所调用的函数，当然一般都是默认的，但是我们可以自己设置这个函数，
* 官方一点的说法在new操作符抛出异常以响应无法满足的内存请求之前，它会调用客户指定的错误处理函数new-handler，
* 首先了解以下代码
*/

namespace std {  // 标准库中
    typedef void (*new_handler)(); // new_handler 是一个函数指针(指向一个无参数无返回值的函数)
    // set_new_handler 顾名思义，返回值指向set_new_handler被调用之前正在执行的那个new_handler
    new_handler set_new_handler(new_handler p) noexcept;
}


/* 一个简单的例子 */

void outOfMem(){ // new_handler
    std::cerr << "Unable to satisfy request for memory\n";
    std::abort(); //如果不终止，程序会一直尝试调用这个处理函数
}

int main(){
    std::set_new_handler(outOfMem);
    for(int i=0; i<100000;i++){  // 当 new 失败的时候会调用我们自己设置的new_handler(outOfMen)
        int* pBigDataArray = new int[100000000L];
    }
}


/*
* 设计良好的new-handler函数做以下事情
*
* 让更多内存可被使用：
* 例如：程序一开始就分配一大块内存，而后当 new-handler 第一次被调用，将它们释还给程序使用。
*
* 安装另一个 new-handler：
* 如果当前 new-handler 无法取得更多可用内存，或许它知道另外哪个 new-handler 有此能力。
* （通过调用set_new_handler，或通过改变全局数据改变自己的行为）
*
* 卸除 new-handler：
* 也就是将 nullptr 传给 set_new_handler，这样 operator new 会在内存分配失败时抛出异常。
*
* 抛出 bad_alloc 或其派生的异常：这样的异常不会被 operator new 捕捉，而会被传播到内存索求处。
*
* 不返回：通常调用 abort 或 exit。
*/



/* 我设计的类，别人可以自定义该类的new_handler,我认为这很酷 */

class X{
public:
    static void outOfMemory();
    //...
}

class Y{
public:
    static void outOfMemory();
    //...
}

X* p1 = new X(); // 如果分配不成功调用 X::outOfMemory
Y* p2 = new Y(); // 如果分配不成功调用 Y::outOfMemory


/* 
* 实现这种方法，只需令每一个 class 提供自己的 set_new_handler 和 operator new 即可，
* 其中 set_new_handler 使客户可以指定 class 专属的 new-handler,operator new则确保在分配
* class 对象内存的过程中以 class 专属的 new-handler 替换 global new-handler
*/


class Widget {
public:
    // 这是 Widget::set_new_handler(你想叫什么叫什么),不是 std::set_new_hander
    static std::new_handler set_new_handler(std::new_handler p) noexcept;
    static void* operator new(std::size_t size);
    ~Widget();
private:
    static std::new_handler currentHandler;
};

std::new_handler Widget::currentHandler = nullptr; // 初始化为空指针
//Widget::set_new_handler 将它获得的函数指针存储起来，然后返回先前存储的指针，这同标准版的行为一样
std::new_handler Widget::set_new_handler(std::new_handler p) noexcept{
    std::new_handler oldHandler = currentHandler;
    currentHandler = p;
    return oldHandler;
}

Widget::~Widget(){
    set
}

/* Widget::operator new 做以下事情  
* 1. 调用 std::set_new_handler,将 Widget::new_handler 安装为 global new_handler
*
* 2. 调用 global operator new, 执行内存分配，若失败，global operator new 调用 Widget::new_handler
*    其实就是当前的 global new_handler. 如果 global operator new 最终分配失败，则会抛出一个 bad_alloc
*	 异常，在此情况下 Widget::operator new 必须恢复原本的 global new_handler
*    (这很合理，因为 Widget::new_handler 并不适应于所有)，然后在抛出该异常，因此我们需要 RAII(见13)
*
* 3. 如果分配成功，Widget::operator new 返回一个指针，指向分配所得,
* 
*/ 

/* 下面运用资源管理类，并实现 Widget::operator new */

class NewHandlerHolder {  // 资源管理类
public:
    explicit NewHandlerHolder(std::new_handler nh) // 获取当前的new-handler
        :handler(nh) {}  
    ~NewHandlerHolder(){std::set_new_handler(handler);}// 释放它

private:
    std::new_handler handler; // 记录下来
    NewHandlerHolder(const NewHandlerHolder&); // 防止拷贝
    NewHandlerHolder& operator=(const NewHandlerHolder&);
};

// Widget::operator new 实现
void* Widget::operator new(std::size_t size) throw(std::bad_alloc){
    /* 得到一个资源管理类 h，并在初始化的时候调用 std::set_new_handler(),所以global new_handler
       已经被替换成 Widget::new_handler，并且 std::set_new_handler() 的返回值(旧的new_handler)
       被储存在 h.handler(资源管理类中) */ 
    NewHandlerHolder h(std::set_new_handler(currentHandler));  
    return ::operator new(size); // 分配内存或抛出异常
} // h 被自动析构，并且调用 std::set_new_handler(handler),恢复全局的new-handler


/* 客户也许会如此调用 */
void outOfMem(); // 函数声明
Widget::set_new_handler(outOfMem); // 将outOfMem设置为Widget的新处理函数
Widget* pw1 = new Widget; // 如果内存分配失败，代用outOfMem
std::string* ps = new std::string; //如果内存分配失败,调用全局新处理函数
Widget::set_new_handler(nullptr); // 设置Widget特定的新处理函数为空
Widget* pw2 = new Widget; // 如果内存分配失败, 立即抛出异常（Widget没有特定的新处理函数）





/* 我们发现不同的类，实现这个方案的代码都是相同的,因此将这部分代码提取出来 */

template<typename T> 
class NewHandlerSupport { 
public:  
    static std::new_handler set_new_handler(std::new_handler p) noexcept;
    static void* operator new(std::size_t size) throw(std::bad_alloc);
    ... // 其他版本的operator new
private:
    static std::new_handler currentHandler;
};

template<typename T>
std::new_handler
NewHandlerSupport<T>::set_new_handler(std::new_handler p) noexcept
{
    std::new_handler oldHandler = currentHandler;
    currentHandler = p;
    return oldHandler;
}

template<typename T>
void* NewHandlerSupport<T>::operator new(std::size_t size)
throw(std::bad_alloc)
{
    NewHandlerHolder h(std::set_new_handler(currentHandler));
    return ::operator new(size);
}

// 这会将每个currentHandler初始化为空指针
template<typename T>
std::new_handler NewHandlerSupport<T>::currentHandler = nullptr;

class Widget : public NewHandlerSupport<Widget> {
    ... // 和之前一样，但没有set_new_handler或operator new的声明
};  

/* 
* 也去你会奇怪为什么使用模板嘞，很简单，我们只需要查看基类中有一个 static std::new_handler currentHandler
* 如果不是模板，那么如果有 Aidget,Bidget 继承它，它们的派生类使用的都是一个 currentHandler(别忘了他是静态) 
* 但是如果是模板，每一种类型都有自己的 currentHandler，这种技巧被称为 "奇异循环模板模式“
*/

````



### 50：了解 new 和 delete 的合理替换时机

````C++
/*
* 编写自定义版本的new和delete有很多合理的理由，包括改进性能、堆使用错误的调试和收集堆使用信息。
* 但是随着 C++ 的发展，没有足够的能力就不要该这些底层的东西，万一考虑不周全
*/

/*
*1、用来检测运用上的错误。
* 如果令operator new持有一串动态分配所得地址，而operator delete将地址从中移除，就很容易检测内存泄露或重复
* delete的错误用法。此外，以额外空间放置特定的 “签名 ”，可以解决“overruns”（写入点在分配区块尾端之后） 和 
* “underruns”（写入点在分配区块起点之前）
*/

static const int signature = 0xDEADBEEF;
typedef unsigned char Byte;
// 这段代码有一些缺陷，例如不能保证内存对齐
void* operator new(std::size_t size) {
    using namespace std;
    size_t realSize = size + 2 * sizeof(int); // 增加请求的大小，所以2个签名也将适合里面
    void* pMem = malloc(realSize); // 调用malloc来获取实际的内存
    if (!pMem) throw bad_alloc(); 
 
    *(static_cast<int*>(pMem)) = signature; //把签名写进内存的头和尾
    *(reinterpret_cast<int*>(static_cast<Byte*>(pMem) + realSize - sizeof(int))) =signature;
    return static_cast<Byte*>(pMem) + sizeof(int);    // 返回一个指向第一个签名后面的内存的指针
}


/*
* 2、为了强化效能。
* 默认的实现对多有情况都相当有效，但对任何情况都不是最佳效果。
*/

/*
* 3、为了收集使用上的统计数据。
* 分配区块的大小分布；寿命分布；它们是倾向于按照FIFO、LIFO的顺序分配和释放，还是更接近随机的顺序？在执行的不同
* 阶段是否有不同的分配/释放模式……
*/

````



### 51：编写 new 和 delete 时需固守常规

````C++
/* C++要求new运算符即使在请求0字节时也返回一个合法的指针 */

void* operator new(std::size_t size) { // 你的new操作符可能需要额外的参数
    using namespace std; 
    if (size == 0) { 	// 处理0字节请求，
        size = 1; 	// 将它们视为1字节请求
    }  
    while (true) {
        ... //尝试分配size个字节;
        if (分配成功)
            return (指向内存的指针);
        // 分配失败;找出当前的new-handler函数
        new_handler globalHandler = get_new_handler(0);
        set_new_handler(globalHandler);
        if (globalHandler) (*globalHandler)();
        else throw std::bad_alloc();
    }
}



/* 成员函数operator new，会被派生类继承 */

class Base {
public:
    static void* operator new(std::size_t size);
    ...
};
class Derived : public Base // 派生类没有声明new运算符
{
    ...
}; 
Derived* p = new Derived; // 调用 Base::operator new!

/* 但派生类和基类的大小可能不同，处理这种情况的最佳方法是将请求“错误”内存大小的调用丢弃给标准new运算符 */

void* Base::operator new(std::size_t size) 
{
    if (size != sizeof(Base)) // 如果大小是“错误的”，
        return ::operator new(size); // 采用标准operator new处理请求
         
    ... // 否则在这里处理请求
}


/*
* 实现operator new[]（array new）时，唯一能做的就是分配一块原始内存，
* 实际上甚至无法计算这个数组将含有多少个元素（当派生类较大时，(请求的字节数)/sizeof(Base) 并不正确；
* 并且数组还可能包含而外的内容，例如数组大小）。
*/

/* 对于delete运算符，事情就简单多了。你需要记住的是，C++保证delete空指针总是安全的，所以你需要遵守这个保证 */

void operator delete(void* rawMemory) noexcept
{
    if (rawMemory == 0) return; // 如果空指针正在被删除，什么都不做
    
    ... // 释放rawMemory指向的内存;
}


/* 成员函数版本也很简单，只需要将大小“错误”的delete行为转交给标准的operator delete */

class Base { // 和之前一样，但现在声明了delete运算符
public:  
    static void* operator new(std::size_t size);
    static void operator delete(void* rawMemory, std::size_t size) noexcept;
    ...
};
void Base::operator delete(void* rawMemory, std::size_t size) noexcept
{
    if (rawMemory == 0) return; // 检查空指针
    if (size != sizeof(Base)) { // 如果大小是“错误的”，使用标准delete运算符
        ::operator delete(rawMemory);  
        return; 
    }
    ... // 释放rawMemory指向的内存;
    return;
}
/* 
* 如果被释放的对象派生自某个缺少虚析构函数的基类，那么 C++ 传给operator delete的size可能不正确，
* 但是只要我们遵守基类的析构函数就是 virtual，就不会有这个问题
*/


/*
* new运算符应该包含一个尝试分配内存的无限循环，如果它不能满足内存请求，应该调用new-handler，
* 应该处理0字节的请求，类专用的版本应该处理比预期更大的块的请求。
* 如果传递给delete运算符的指针是null，那么它什么也不会做。类专用版本应该处理比预期更大的块。
*/

````



### 52：写了 placement new 也要写 placement delete

````C++
/* 下面的代码调用了两个函数：1、调用operator new来分配内存；2、调用Widget的默认构造函数 */

Widget *pw = new Widget;

/*  
* 如果第2步发生异常，撤销第1步的责任必须落在C++运行时系统上，因为客户就没有取得指针 
* 如果你正在处理具有普通签名的new和delete版本，这不是问题，
* 有一种特别有用的定位new，它接受一个指针，指定在何处构建对象
*/

// 这个版本的new是C++标准库的一部分，当#include <new>时，便可以访问它。
void* operator new(std::size_t, void* pMemory) noexcept; // “定位new"


/* 假设写了一个类专用new运算符，要求指定一个ostream，并记录分配信息，还写了一个普通的类专用delete运算符 */
class Widget {
public:
    ...
    static void* operator new(std::size_t size, std::ostream& logStream);// 非普通的new
    static void operator delete(void* pMemory, std::size_t size) noexcept); // 普通的类专属的delete 
    ...
}; /* 运行时系统会寻找一个delete运算符的版本，它需要接受与new运算符相同数量和类型的额外参数
（void operator delete(void *, std::ostream&) noexcept;）*/

Widget* pw = new (std::cerr) Widget; // 如果Widget构造函数抛出异常，就会泄漏内存 


/* 如果有一个基类，只声明了定位版本的new，会发现普通的new不可用了 */

class Base {
public:
    ...
   // 这个new隐藏了全局的普通new
    static void* operator new(std::size_t size,  std::ostream& logStream);  
    ...
};
Base* pb = new Base; // 错误! 普通new运算符被隐藏了
Base* pb = new (std::cerr) Base; // 没问题，调用Base类的定位new



/* 类似地，派生类中的 new 会同时隐藏全局版本和继承版本的 new */

class Derived : public Base { 
public:
    ...
    static void* operator new(std::size_t size); // 重新声明普通的new
    ...
};
Derived* pd = new (std::clog) Derived; // 错误! Base的定位new被隐藏了
Derived* pd = new Derived; // 没问题, 调用 Derived的new


/* 如果希望这些函数像往常一样工作，只需要让特定于类的版本调用全局版本 */
//C++在全局作用域中提供了下列形式的new运算符：
void* operator new(std::size_t); // 普通 new
void* operator new(std::size_t, void*) noexcept; // 定位 new
void* operator new(std::size_t,const std::nothrow_t&) noexcept;//nothrow new

class StandardNewDeleteForms { //基类提供所有的版本
public:
     普通 new/delete
    static void* operator new(std::size_t size){return ::operator new(size);}
    static void operator delete(void* pMemory) noexcept{::operator delete(pMemory);}
    // 定位 new/delete
    static void* operator new(std::size_t size, void* ptr) noexcept{return ::operator new(size, ptr);}
    static void operator delete(void* pMemory, void* ptr) noexcept
    {return ::operator delete(pMemory, ptr);}
    // nothrow new/delete
    static void* operator new(std::size_t size, const std::nothrow_t& nt) noexcept
    {return ::operator new(size, nt);}
    static void operator delete(void* pMemory, const std::nothrow_t&) noexcept
    {::operator delete(pMemory);}
};


/* 或者 */
class Widget : public StandardNewDeleteForms { // 继承所有标准的形式
public:
    using StandardNewDeleteForms::operator new; // 让这些形式可见
    using StandardNewDeleteForms::operator delete;  
    static void* operator new(std::size_t size,  std::ostream& logStream) ;// 添加一个自定义定位new
    // 添加相应的定位delete
    static void operator delete(void* pMemory,  std::ostream& logStream)  noexcept; 
    ...
};

/*
* 在编写定位版本的new运算符时，一定要编写相应的定位版本的delete运算符。如果不这样做，程序可能会遇到微妙的、
* 间歇性的内存泄漏。当你声明new和delete的定位版本时，请确保不要无意中隐藏了这些函数的普通版本。
*/

````





## 九、杂项讨论



### 53：不要轻忽编译器的警告

````C++
/* 下面是每个人都可能犯过的错误 */
class B {
public:
    virtual void f() const;
};
class D : public B {
public:
    virtual void f(); // 有没有const是两个完全不同的版本，并且D::f 将遮掩 B::f
};

/* 编译器：警告！在B中声明的f没有在D中重新声明;相反，它被完全隐藏起来了 */

/*
* 认真对待编译器警告，并努力在编译器支持的最大警告级别上编译无警告。
* 不要过于依赖编译器的警告，因为不同的编译器对待警告的态度并不相同。移植到新的编译器可能会消除你所依赖的警告消息。
*/

````



### 54：让自己熟悉包括 TR1 在内的所有标准程序库

````C++
/* TR1 (“Technical Report 1” )是一个临时版本，现在已经完全融入标准C++ */

````



### 55：让自己熟悉 Boost

```C++
/*
* Boost是一个社区和网站，用于开发免费的、开源的、经过同行评审的C++库。Boost在C++标准化中发挥着重要作用。
* Boost提供了许多TR1组件的实现，但它也提供了许多其他库。
*/

```

















