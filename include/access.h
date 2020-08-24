struct Access
{
    template <typename Serializer, typename T>
    static void serialize(Serializer &s, T &t)
    {
        t.serialize(s);
    }
};