


using Actions.Library;
using System.Runtime.CompilerServices;

public static class Application {
    
    static string CurrentFileDir([CallerFilePath]string filePath = null) {
        return System.IO.Path.GetDirectoryName(filePath);
    }

    static string GetProjectDir() {
        return System.IO.Path.GetDirectoryName(CurrentFileDir());
    }


    public static int Main() {

        var projectDir = GetProjectDir();

        var buildDir = TempDirectory.New();
        CMake.Configure(projectDir, buildDir);
        CMake.Build(buildDir);

        var tmpDir = Actions.Library.TempDirectory.New();


        return 0;
    }
}