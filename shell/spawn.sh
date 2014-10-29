printf "\nstarting where root directory is $1\n\n"

spawn-fcgi -p 8000 $1/src/fastcgi_app.out
