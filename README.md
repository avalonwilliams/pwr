# Pwr
This project prints power statistics for GNU/Linux and FreeBSD

## Install
### GNU/Linux
(Optional) Install ronn to make man pages, and run `make docs` before installing

Clone & cd into repo, then use GNU Make to install:
```
make 
sudo make install
```

### FreeBSD
(Optional) Install ronn to make man pages, and run `make docs` before installing:
```
pkg install rubygem-ronn
```

Install dependencies:
```
pkg install libexplain
```

Clone & cd into repo, then use GNU Make to install:
```
gmake && sudo gmake install
```

## License
This project is available under the Gplv3 or above.
