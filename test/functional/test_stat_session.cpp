#include "test_stat_session.h"

#include <core.h>
#include <http_backend.h>
#include <glibmm/init.h>

using namespace Davix;

int n_call=0;

int mycred_auth_callback(davix_auth_t token, const davix_auth_info_t* t, void* userdata, GError** err){
    GError * tmp_err=NULL;
    int ret = davix_set_pkcs12_auth(token, (const char*)userdata, (const char*)NULL, &tmp_err);
    if(ret != 0){
        fprintf(stderr, " FATAL authentification Error : %s", tmp_err->message);
        g_propagate_error(err, tmp_err);
    }

    n_call+=1;
    return ret;
}


static void configure_grid_env(char * cert_path, const Glib::RefPtr<Core>  & core){
    AbstractSessionFactory* f = core->getSessionFactory();
    f->set_ssl_ca_check(false);            // disable ssl ca check
    f->set_authentification_controller(cert_path, &mycred_auth_callback);
}

int main(int argc, char** argv){
    if( argc < 3){
        std::cout << "Usage  : " << std::endl;
        std::cout <<"\t" << argv[0] << " [CERTIFICATE_PATH] [url] [url] ....  " << std::endl;
        return 0;
    }

    g_logger_set_globalfilter(G_LOG_LEVEL_MASK);

    try{
        Glib::RefPtr<Core> c= Core::create(new NEONSessionFactory());
        if(argc > 2){
            configure_grid_env(argv[1], c);
        }


        for(int i =2 ; i< argc; ++i){
            struct stat st;
            c->stat(argv[i], &st);

            std::cout << "stat success" << std::endl;
            std::cout << " atime : " << st.st_atime << std::endl;
            std::cout << " mtime : " << st.st_mtime << std::endl;
            std::cout << " ctime : " << st.st_ctime << std::endl;
            std::cout << " mode : 0" << std::oct << st.st_mode << std::endl;
            std::cout << " len : " << st.st_size << std::endl;
        }



        std::cout << "authentification callback has been called " << n_call << std::endl;
    }catch(Glib::Error & e){
        std::cout << " error occures : N°" << e.code() << "  " << e.what() << std::endl;
        return -1;
    }catch(std::exception & e){
        std::cout << " error occures :" << e.what() << std::endl;
        return -1;
    }
    return 0;
}
