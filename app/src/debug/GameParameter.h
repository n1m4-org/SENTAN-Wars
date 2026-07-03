#pragma once
#include <debug/DebugEntryManager.h>
#include <debug/DebugEntry.h>
#include <source_location>
#include <functional>
#include <string>

#define GameParameter(type, name, value)\
    GameParameterData<type> name{std::source_location::current().file_name(), #name, value}

#ifdef _DEBUG

#define GameParameterView(type, name, value) \
    type name = value; \
    GameParameterViewData<type> view_##name{std::source_location::current().file_name(), #name, &name}

    #define EnableDebug(category) DebugEntry debugEntry{ std::source_location::current().file_name(), category }

    template <typename ValueType>
    class GameParameterData
    {
    public:
        using OnChangeCallback = std::function<void(const ValueType&)>;

        GameParameterData(
            const std::string& id,
            const std::string& name,
            ValueType&& value = {})
            : v_(std::move(value))
        {
            DebugEntryManager::GetInstance()->HandleParameter(id, name, &v_, [this]()
            {
                if (onChange_)
                {
                    onChange_(v_);
                }
            });
        }

        GameParameterData(const GameParameterData&) = delete;
        GameParameterData& operator=(const GameParameterData&) = delete;

        ValueType& operator=(const ValueType& newValue)
        {
            v_ = newValue;
            return v_;
        }
        operator ValueType& () { return v_; }
        operator const ValueType& () const { return v_; }
        ValueType* operator-> () { return &v_; }    
        const ValueType* operator-> () const { return &v_; }

        ValueType* GetPtr() { return &v_; }
        ValueType& Get() { return v_; }
        const ValueType& Get() const { return v_; }

        void SetOnChange(OnChangeCallback cb) { onChange_ = std::move(cb); }

    private:
        ValueType v_ = {};
        OnChangeCallback onChange_;
    };

    template <typename ValueType>
    class GameParameterViewData
    {
    public:
        GameParameterViewData(
            const std::string& id,
            const std::string& name,
            const ValueType* pValue = nullptr)
        {
            DebugEntryManager::GetInstance()->HandleParameter(id, name, pValue);
        }
    };


#else
    #define GameParameterView(type, name, value) \
        type name = value;
    #define EnableDebug(category)

    template <typename ValueType>
    class GameParameterData
    {
    public:
        GameParameterData(const std::string&, const std::string&, ValueType&& value = {}) : v_(std::move(value)) {}
        GameParameterData(const GameParameterData&) = delete;
        GameParameterData& operator=(const GameParameterData&) = delete;
        ValueType& operator=(const ValueType& newValue)
        {
            v_ = newValue;
            return v_;
        }
        operator ValueType& () { return v_; }
        operator const ValueType& () const { return v_; }
        ValueType* operator-> () { return &v_; }
        const ValueType* operator-> () const { return &v_; }
        ValueType* GetPtr() { return &v_; }
        ValueType& Get() { return v_; }
        const ValueType& Get() const { return v_; }
    private:
        ValueType v_ = {};
    };

#endif