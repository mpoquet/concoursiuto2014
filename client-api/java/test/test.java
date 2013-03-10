public class test
{
    static
    {
        System.loadLibrary("contest");
    }

    public static void main(String[] args)
    {
        Session session1 = new Session("a.b.c.d", 1080);
        System.out.println(session1.isValide());
        System.out.println(session1.lastError());

        Session session2 = new Session("127.0.0.1", 1080);
        System.out.println(session2.isValide());
        System.out.println(session2.lastError());
    }
}

