# How to build and run project?

You need to use simple Python script. Just run it `python manage.py` with one of following arguments:

- `build` generate our disk image
- `run` run generated disk image in bochs
- `test` run `build` and `run`
- `clean` - empty temp directory

# What is in project's directories?

- /src     - Code
- /scripts - Building scripts in python
- /conf    - Bochs config
- /bin     - Generated disk image
- /tmp     - Directory with temporary files
