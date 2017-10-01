#include <cstddef>
#include <cassert>
#include <tuple>

template<size_t a, size_t b>
struct greater {
    static constexpr bool value = a > b;
};

template<typename... T>
struct list;

template<>
struct list<> {
};

template<typename Head, typename... Tail>
struct list<Head, Tail...> {
    constexpr list(Head h, Tail... t) : head(std::move(h)), tail(std::move(t)...) {}
    constexpr list(Head h, list<Tail...> t) : head(std::move(h)), tail(std::move(t)) {}
    Head head;
    list<Tail...> tail;
};

template<typename T>
struct is_list : std::false_type {};

template<typename... T>
struct is_list<list<T...>> : std::true_type {};

template<typename T>
struct list_size {
    static_assert(is_list<T>::value);
};

template<typename... T>
struct list_size<list<T...>> : std::integral_constant<size_t, sizeof...(T)> {};

template<typename Head, typename... Tail>
constexpr Head& get(list<Head, Tail...>& l, std::integral_constant<size_t, 0>) {
    return l.head;
}

template<size_t i, typename Head, typename... Tail>
constexpr auto get(list<Head, Tail...>& l, std::integral_constant<size_t, i>) {
    return get(l.tail, std::integral_constant<size_t, i-1>());
}

template<size_t i, typename Head, typename... Tail>
constexpr auto get(list<Head, Tail...>& l) {
    return get(l, std::integral_constant<size_t, i>());
}

template<typename Head, typename... Tail>
constexpr const Head& get(const list<Head, Tail...>& l, std::integral_constant<size_t, 0>) {
    return l.head;
}

template<size_t i, typename Head, typename... Tail>
constexpr auto get(const list<Head, Tail...>& l, std::integral_constant<size_t, i>) {
    return get(l.tail, std::integral_constant<size_t, i-1>());
}

template<size_t i, typename Head, typename... Tail>
constexpr auto get(const list<Head, Tail...>& l) {
    return get(l, std::integral_constant<size_t, i>());
}

template<typename Head>
constexpr auto make_list(const Head& h, const list<>&) {
    return list<Head>(h);
}

template<typename Head, typename... Tail>
constexpr auto make_list(const Head& h, const list<Tail...>& t) {
    return list<Head, Tail...>(h, t);
}

constexpr auto make_list() {
    return list<>();
}

template<typename Head, typename... Tail>
constexpr auto make_list(const Head& h, const Tail&... t) {
    return list<Head, Tail...>(h, t...);
}

template<typename... T>
constexpr auto append(const list<>&, const list<T...>& l2) {
    return l2;
}

template<typename Head1, typename... Tail1, typename... T>
constexpr auto append(const list<Head1,Tail1...>& l1, const list<T...>& l2) {
    return make_list(l1.head, append(l1.tail, l2));
}

using Symbol = char;
using State = size_t;
using TokenId = size_t;
struct transition {
    constexpr transition(Symbol c_, State s_) : c(c_), s(s_) {}
    Symbol c;
    State s;
};

template<typename T>
struct is_transitions : std::false_type {};

template<>
struct is_transitions<list<>> : std::true_type {};

template<typename... Tail>
struct is_transitions<list<transition,Tail...>> {
    static constexpr bool value = is_transitions<list<Tail...>>::value;
};

template<typename T, typename Enable = void>
struct transitions {
    static_assert(is_transitions<T>::value);
};

template<typename T>
struct transitions<T, std::enable_if_t<is_transitions<T>::value>>  {
    T l;
    constexpr transitions(const T& l_) : l(l_) {}
};

template<typename T, typename Enable = void>
struct node {
    static_assert(is_transitions<T>::value);
};

template<typename T>
struct node<T, std::enable_if_t<is_transitions<T>::value>> {
    TokenId id;
    transitions<T> ts;
    constexpr node(TokenId id_, const transitions<T>& ts_) : id(id_), ts(ts_) {}
};

template<typename T>
struct is_node : std::false_type {};

template<typename T>
struct is_node<node<T>> : is_transitions<T> {};

template<typename T>
struct is_nodes : std::false_type {};

template<>
struct is_nodes<list<>> : std::true_type {};

template<typename Head, typename... Tail>
struct is_nodes<list<Head,Tail...>> {
    static constexpr bool value = is_node<Head>::value && is_nodes<list<Tail...>>::value;
};

template<typename T, typename Enable = void>
struct node_vector {
    static_assert(is_nodes<T>::value);
};

template<typename T>
struct node_vector<T, std::enable_if_t<is_nodes<T>::value>> {
    T l;
    size_t entry, exit;
    constexpr node_vector(const T& ts, size_t entry_, size_t exit_) : l(ts), entry(entry_), exit(exit_) {}
};

template<typename T>
struct is_node_vector : std::false_type {};

template<typename T>
struct is_node_vector<node_vector<T>> : is_nodes<T> {};

static constexpr auto eps = 0u;

//template<typename... T>
//constexpr std::enable_if_t<is_transitions<list<T...>>::value,transitions<list<T...>>>
//make_transitions(const T&... args) {
//    return make_list(args...);
//}

template<typename... T>
constexpr std::enable_if_t<is_transitions<list<T...>>::value,transitions<list<transition, T...>>>
make_transitions(const transition& head, const list<T...>& tail) {
    return make_list(head, tail);
}

template<typename T>
constexpr std::enable_if_t<is_transitions<T>::value,transitions<T>>
make_transitions(const T& l) {
    return l;
}

template<typename... T>
constexpr std::enable_if_t<is_transitions<list<T...>>::value,transitions<list<transition, T...>>>
make_transitions(const transition& head, const transitions<list<T...>>& tail) {
    return make_list(head, tail.l);
}

template<typename T>
constexpr std::enable_if_t<is_transitions<T>::value,node<T>>
make_node(TokenId tokenId, const transitions<T>& ts) {
    return {tokenId, ts};
}

//template<typename... T>
//constexpr std::enable_if_t<is_nodes<list<T...>>::value,node_vector<list<T...>>>
//make_node_vector(const T&... args) {
//    return make_list(args...);
//}

template<typename Transitions, typename... Nodes>
constexpr std::enable_if_t<is_transitions<Transitions>::value && is_nodes<list<Nodes...>>::value,node_vector<list<node<Transitions>, Nodes...>>>
make_node_vector(const node<Transitions>& head, const list<Nodes...>& tail, size_t entry, size_t exit) {
    return {make_list(head, tail), entry, exit};
}

template<typename Nodes>
constexpr std::enable_if_t<is_nodes<Nodes>::value,node_vector<Nodes>>
make_node_vector(const Nodes& l, size_t entry, size_t exit) {
    return {l, entry, exit};
}

template<typename Transitions, typename... Nodes>
constexpr std::enable_if_t<is_transitions<Transitions>::value && is_nodes<list<Nodes...>>::value,node_vector<list<node<Transitions>, Nodes...>>>
make_node_vector(const node<Transitions>& head, const node_vector<list<Nodes...>>& tail, size_t entry, size_t exit) {
    return {make_list(head, tail.l), entry, exit};
}

struct single {
    constexpr single(char c) : c_(c) {}
    char c_;
};

template<size_t nextState>
constexpr auto make(const single& s) {
    return make_node_vector(make_list(make_node(0, make_transitions(make_list(transition(s.c_, nextState+1)))), make_node(0, make_transitions(make_list()))), nextState, nextState+1);
}

template<typename T, typename = std::enable_if_t<is_nodes<T>::value>>
constexpr auto add_transition(const T& x, const transition& t, std::integral_constant<size_t, 0>) {
    return make_list(make_node(x.head.id, make_transitions(t, x.head.ts.l)), x.tail);
}

template<size_t i, typename T, typename = std::enable_if_t<is_nodes<T>::value>>
constexpr auto add_transition(const T& x, const transition& t, std::integral_constant<size_t, i>) {
    return make_list(x.head, add_transition(x.tail, t, std::integral_constant<size_t, i-1>()));
}

template<size_t i, typename T, typename = std::enable_if_t<is_nodes<T>::value>>
constexpr auto add_transition(const T& x, const transition& t) {
    return add_transition(x, t, std::integral_constant<size_t, i>());
}

template<typename E, typename Enabled = void>
struct cat_ {
    static_assert(list_size<E>::value > 1);
};

template<typename E>
struct cat_<E, std::enable_if_t<greater<list_size<E>::value, 1>::value>> {
    constexpr cat_(const E& e) : e_(e) {}
    E e_;
};
    
template<size_t nextState>
constexpr auto cat_helper(const list<>&) {
    return make_list();
}

template<size_t nextState, typename Head, typename... Tail>
constexpr auto cat_helper(const list<Head, Tail...>& l) {
    auto t = make<nextState>(l.head);
    static_assert(is_node_vector<decltype(t)>::value);
    constexpr auto s1 = list_size<decltype(t.l)>::value;
    constexpr auto s = nextState + s1;
    return append(add_transition<s1-1>(t.l, {eps, s}), cat_helper<s>(l.tail));
}

template<size_t nextState, typename E>
constexpr auto make(const cat_<E>& c) {
    auto t = cat_helper<nextState>(c.e_);
    return make_node_vector(t, nextState, nextState + list_size<decltype(t)>::value - 1);
}

template<typename E>
constexpr auto cat(const E& e, std::enable_if_t<is_list<E>::value && greater<list_size<E>::value,1>::value>* = 0) {
    return cat_<E>(e);
}

//template<typename E>
//struct closure_ {
//    constexpr closure_(const E& e) : e_(e) {}
//    constexpr auto operator() const (size_t nextState) { return make_node_vector(); }
//    E e_;
//};
//
//template<typename E>
//constexpr auto closure(const E& e) {
//    return closure_<E>(e);
//}

template<typename E1, typename E2>
struct alt_ {
    constexpr alt_(const E1& e1, const E2& e2) : e1_(e1), e2_(e2) {}
    E1 e1_;
    E2 e2_;
};

template<size_t nextState, typename E1, typename E2, alt_<E1,E2> a>
constexpr auto make() {
    constexpr auto entry = nextState;
    constexpr auto exit = nextState + 1;
    constexpr auto t = make<nextState+2>(a.e1_);
    static_assert(is_node_vector<decltype(t)>::value);
    auto t1 = make_node_vector(add_transition<t.exit>(t.l, {eps, exit}), t.entry, t.exit);
    auto tt = make<nextState+2+list_size<decltype(t1.l)>::value>(a.e2_);
    static_assert(is_node_vector<decltype(tt)>::value);
    auto t2 = make_node_vector(add_transition<tt.exit>(tt, {eps, exit}), tt.entry, tt.exit);
    return make_node_vector(make_list(make_node(0, make_transitions(make_list(transition(eps, t1.entry), transition(eps, t2.entry)))), make_list(make_node(0, make_transitions(make_list())), append(t1, t2))), entry, exit);
}

//template<typename E>
//struct alt_<E, std::enable_if_t<greater<list_size<E>::value, 1>::value>> {
//    constexpr alt_(const E& e) : e_(e) {}
//
//    template<size_t nextState, typename Head>
//    constexpr auto helper(const list<Head>& l) const {
//        return l.head.template make<nextState>();
//    }
//
//    template<size_t nextState, typename Head, typename... Tail>
//    constexpr auto helper(const list<Head,Tail...>& l) const {
//        constexpr auto entry = nextState;
//        constexpr auto exit = nextState + 1;
//        auto t = l.head.template make<nextState+2>();
//        static_assert(is_node_vector<decltype(t)>::value);
//        auto t1 = add_transition<t.exit>(t.l, {eps, exit});
//        constexpr auto start_of_t1 = t1.entry;
//        constexpr auto end_of_t1 = t1.exit;
//        auto tt = helper<nextState+2+list_size<decltype(t1.l)>::value>(l.tail);
//        auto t2 = add_transition<tt.exit>(tt, {eps, exit});
//        constexpr auto start_of_t2 = ;
//        constexpr auto end_of_t2 = ;
//        constexpr auto s1 = ;
//        constexpr auto s = nextState + s1;
//        return make_node_vector(make_list(make_node(0, make_transitions(make_list(transition(eps, nextState+2), transition(eps, )))), make_list(make_node(0, make_transitions(make_list())), append(t1, t2))));
//    }
//
//    template<size_t nextState>
//    constexpr auto make() const {
//        return helper<nextState>(e_);
//    }
//    E e_;
//};

template<typename E1, typename E2>
constexpr auto alt(const E1& e1, const E2& e2) {
    return alt_<E1, E2>(e1, e2);
}

//template<typename E>
//constexpr auto alt(const E& e, std::enable_if_t<is_list<E>::value && greater<list_size<E>::value,1>::value>* = 0) {
//    return alt_<E>(e);
//}

constexpr single string(const char* s, std::integral_constant<size_t, 1>) {
    return single(s[0]);
}

template<size_t n>
constexpr auto string(const char* s, std::integral_constant<size_t, n>) {
    return cat(make_list(single(s[0]), string(s+1, std::integral_constant<size_t, n-1>())));
}

template<size_t n>
constexpr auto string(const char(&s)[n], std::enable_if_t<greater<n,1>::value>* = 0) {
    return string(s, std::integral_constant<size_t, n-1>());
}

constexpr auto myNFA() {
    //return alt(make_list(string("true"), string("false"))).make<0>();
    constexpr auto bla = alt(string("true"), string("false"));
    return make<0>(bla);
}

int main() {
    constexpr auto nfa = myNFA();
    static_assert(list_size<decltype(nfa.l)>::value == 18);
    return 0;
}
