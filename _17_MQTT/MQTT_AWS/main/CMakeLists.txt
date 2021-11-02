set(COMPONENT_SRCS "main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_EMBED_TXTFILES
    "../certs/aws_root_ca.pem"
    "../certs/certificate_pem.crt"
    "../certs/private_pem.key"
)

register_component()