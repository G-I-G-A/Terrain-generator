#ifndef SIGNAL_H
#define SIGNAL_H

#include <iostream>
#include <functional>
#include <variant>
#include <vector>
#include <stdexcept>

template<typename... Args>
class Signal;

template<typename... Types>
struct overload : public Types...
{
    using Types::operator()...;
    overload(Types... types) : Types(types)... {}
};

template<typename... Types>
overload(Types...) -> overload<Types...>;

template<typename Variant, typename... Matchers>
void checkState(Variant&& variant, Matchers&&... matchers)
{
    std::visit(overload(std::forward<Matchers>(matchers)...), std::forward<Variant>(variant));
}

namespace details
{
    template<typename... Args>
    struct connection
    {
        using slot_type = std::function<void(Args...)>;
        using signal_type = Signal<Args...>;
        using signal_type_ptr = signal_type*;

        struct managed {};
        struct scoped {};
        struct undead {};
        using state_type = std::variant<managed, scoped, undead>;

        connection(signal_type_ptr signal_, const slot_type& slot_, const state_type& state_)
            : signal(signal_)
            , slot(slot_)
            , state(state_)
        {}

        signal_type_ptr signal;
        slot_type slot;
        state_type state;
    };

    template<typename... Args>
    class scoped_connection
    {
        friend Signal<Args...>;
    public:

        using connection_type = connection<Args...>;
        using connection_type_ptr = connection_type*;
        using signal_type = typename connection_type::signal_type;
        using fn_type = typename connection_type::slot_type;

        ~scoped_connection()
        {
            disconnect();
        }

    private:
        explicit scoped_connection(connection_type_ptr connection_)
            : m_connection(connection_)
        {}

        void disconnect() const
        {
            if (m_connection)
                m_connection->signal->disconnect(m_connection);
        }

        mutable connection_type_ptr m_connection;
    };

    template<typename... Args>
    class slot_connection
    {
    public:
        using connection_type = scoped_connection<Args...>;
        using connection_type_ptr = connection_type*;
        using signal_type = typename connection_type::signal_type;
        using fn_type = typename connection_type::fn_type;
        using current_type = slot_connection<Args...>;

        slot_connection(signal_type& signal, const fn_type& fn)
            : m_semaphore{}
            , m_scopedConnection(signal.connectScoped([this, fn](Args... args)
                {
                    if (m_semaphore.isEnabled())
                        return;

                    fn(args...);
                }))
        {

        }

        void block()
        {
            m_semaphore.addBlocker();
        }

        void unblock()
        {
            m_semaphore.removeBlocker();
        }

        auto scopedBlock()
        {
            struct Tmp
            {
                explicit Tmp(current_type& owner_)
                    : owner(owner_)
                {
                    owner.block();
                }

                ~Tmp()
                {
                    owner.unblock();
                }

                current_type& owner;
            };

            return Tmp{ *this };
        }

        slot_connection(const current_type&) = delete;
        slot_connection(current_type&&) = delete;
        slot_connection& operator=(const current_type&) = delete;
        slot_connection& operator=(current_type&&) = delete;

    private:
        class semaphore
        {
        public:
            semaphore()
                : m_counter(0)
            {}

            void addBlocker()
            {
                ++m_counter;
            }

            void removeBlocker()
            {
                --m_counter;
            }

            bool isEnabled() const
            {
                return m_counter != 0;
            }

        private:
            size_t m_counter;
        };

        semaphore m_semaphore;
        connection_type m_scopedConnection;
    };
}

template<typename... Args>
class Signal
{
public:
    using connection_type = details::connection<Args...>;
    using connection_type_ptr = connection_type*;
    using fn_type = typename connection_type::slot_type;
    using state_type = typename connection_type::state_type;
    using current_type = Signal<Args...>;
    using connections = std::vector<connection_type_ptr>;
    using scoped_connection_type = details::scoped_connection<Args...>;
    using slot_connection_type = details::slot_connection<Args...>;

    Signal() = default;

    ~Signal()
    {
        for (auto it = m_connections.rbegin(); it != m_connections.rend(); ++it)
        {
            checkState((*it)->state,
                [&](typename connection_type::managed)
                {
                    delete (*it);
                },
                [&](typename connection_type::scoped)
                {
                    (*it)->state = typename connection_type::undead{};
                },
                [&](typename connection_type::undead)
                {
                    delete (*it);
                }
            );
        }
    }

    void connect(const fn_type& fn) const
    {
        auto* connection = new connection_type(const_cast<current_type*>(this), fn,
            state_type{ typename connection_type::managed{} });

        m_connections.push_back(connection);
    }

    template<typename InstanceType>
    void connect(InstanceType* instance, void (InstanceType::* member)(Args...)) const
    {
        auto fn = [instance, member](Args... args)
            {
                (instance->*member)(args...);
            };

        connect(fn);
    }

    template<typename InstanceType>
    void connect(InstanceType* instance, void (InstanceType::* member)(Args...) const) const
    {
        auto fn = [instance, member](Args... args)
            {
                (instance->*member)(args...);
            };

        connect(fn);
    }

    scoped_connection_type connectScoped(const fn_type& fn) const
    {
        auto* connection = new connection_type(const_cast<current_type*>(this), fn,
            state_type{ typename connection_type::scoped{} });

        m_connections.push_back(connection);

        return scoped_connection_type{ connection };
    }

    template<typename InstanceType>
    scoped_connection_type connectScoped(InstanceType* instance, void(InstanceType::* member)(Args...)) const
    {
        auto fn = [instance, member](Args... args)
            {
                (instance->*member)(args...);
            };

        return connectScoped(fn);
    }

    template<typename InstanceType>
    scoped_connection_type connectScoped(InstanceType* instance, void(InstanceType::* member)(Args...) const) const
    {
        auto fn = [instance, member](Args... args)
            {
                (instance->*member)(args...);
            };

        return connectScoped(fn);
    }

    slot_connection_type connectSlot(const fn_type& fn) const
    {
        return slot_connection_type{ *const_cast<current_type*>(this), fn };
    }

    void notify(const Args&... args) const
    {
        auto connections = m_connections;
        for (auto& connection : connections)
            connection->slot(args...);
    }

    void disconnect(connection_type_ptr connection) const
    {
        auto it = std::find(m_connections.begin(), m_connections.end(), connection);
        if (it == m_connections.end())
            throw std::runtime_error("Connection is not attached to the signal");

        checkState(connection->state,
            [&](typename connection_type::managed)
            {
                throw std::runtime_error("Connection is managed!!!");
            },
            [&](typename connection_type::scoped)
            {
                m_connections.erase(it);
            },
            [&](typename connection_type::undead)
            {
            }
        );

        delete connection;
        connection = nullptr;
    }

private:

    mutable connections m_connections;
};


struct Object
{
    using SlotConnection = details::slot_connection<int, int>;
    Object(Signal<int, int>& signal)
        : connection(signal.connectSlot([this](int a, int b)
            {
                method(a, b);
            }))
    {

    }

    void method(int a, int b)
    {
        std::cout << "From method: " << a + b << std::endl;
    }
    SlotConnection connection;
};


#endif // SIGNAL_H