


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


        var artifactsOutputDirectory = System.Environment.GetEnvironmentVariable("ARTIFACTS_OUTPUT_DIRECTORY");
        if (string.IsNullOrEmpty(artifactsOutputDirectory)) {
            artifactsOutputDirectory = Path.Combine(projectDir, "LocatBuildArtifacts");
        }

        if(Directory.Exists(artifactsOutputDirectory)) {
            Directory.Delete(artifactsOutputDirectory, true);
        }

        Directory.CreateDirectory(artifactsOutputDirectory);


        var srcFile = Path.Combine(buildDir, "bin", "ComInterfaceGenerator.exe");
        File.Copy(srcFile, Path.Combine(artifactsOutputDirectory, "ComInterfaceGenerator.exe"));


        return 0;
    }
}