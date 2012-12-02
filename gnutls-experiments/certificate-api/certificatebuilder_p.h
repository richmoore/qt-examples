// -*- c++ -*-

#ifndef CERTIFICATEBUILDER_P_H
#define CERTIFICATEBUILDER_P_H

#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

#include "certificatebuilder.h"

struct CertificateBuilderPrivate
{
    int errno;
    gnutls_x509_crt_t crt;
};

#endif // CERTIFICATEBUILDER_P_H

