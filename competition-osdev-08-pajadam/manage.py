from scripts import build, run
import shutil
import sys
import os


def main():

    try:
        # Add missing directories if needed
        for directory in ("tmp", "bin",):
            if not os.path.exists(directory):
                os.makedirs(directory)

        # Parse console arguments
        if len(sys.argv) == 2:
            os.chdir("tmp/")
            if sys.argv[1] == "build":
                build()
            elif sys.argv[1] == "run":
                run()
            elif sys.argv[1] == "test":
                build()
                run()
            elif sys.argv[1] == "clean":
                os.chdir("..")
                shutil.rmtree("tmp")
                print "Cleaned ./tmp directory."
            else:
                raise ValueError("Invalid argument: " + sys.argv[1])
        else:
            raise ValueError("Missing arguments")

    except ValueError as exc:
        print exc
        print "Usage:"
        print "  build\t- Build project"
        print "  run\t- Run project"
        print "  test\t- Build and run project"
        print "  clean\t- Clean temporary directory\n"

    except Exception as exc:
        print exc


if __name__ == "__main__":
    main()
