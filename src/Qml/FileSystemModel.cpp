#include "FileSystemModel.h"

Qml::FileSystemModel::FileSystemModel(std::shared_ptr<::FileSystemModel> model) : _fs_model(std::move(model)) {
    _fs_model->set_error_func(std::bind(&FileSystemModel::handle_error, this, std::placeholders::_1, std::placeholders::_2));
    _fs_model->add_notification_func(this, std::bind(&FileSystemModel::replyGot, this));
}

Qml::FileSystemModel::~FileSystemModel() {
    _fs_model->remove_notification_func(this);
    _fs_model->set_error_func(nullptr);
}

void Qml::FileSystemModel::setRootPath(const QString& absolute_path) { _fs_model->set_root_path(absolute_path); }

void Qml::FileSystemModel::setServerInfo(const QString& addr, uint16_t port) { _fs_model->set_server_info(addr, port); }

void Qml::FileSystemModel::requestFileList(const QString& relative_path) {
    maxProgressEnabled(false);
    progressTextChanged(tr("Getting the list of files…"));
    _fs_model->request_file_list(relative_path);
}

void Qml::FileSystemModel::abortRequest() { _fs_model->abort_request(); }

void Qml::FileSystemModel::disconnect() { _fs_model->disconnect(); }

QString Qml::FileSystemModel::getCurrentPath() const { return _fs_model->get_current_path(); }

void Qml::FileSystemModel::handle_error(::FileSystemModel::Error custom_error, QNetworkReply::NetworkError qt_error) {
    if (custom_error == ::FileSystemModel::Error::ReplyParseError) {
        errorOccurred(QObject::tr("Reply parse error"));
        return;
    }
    assert(qt_error != QNetworkReply::NoError);
    QString display_str;
    QString log_str;
    switch (qt_error) {
        case QNetworkReply::ConnectionRefusedError:
            display_str = QObject::tr("Connection refused");
            log_str = "QNetworkReply::ConnectionRefusedError";
            break;

        case QNetworkReply::RemoteHostClosedError:
            display_str = QObject::tr("Connection closed");
            log_str = "QNetworkReply::RemoteHostClosedError";
            break;

        case QNetworkReply::HostNotFoundError:
            display_str = QObject::tr("Host not found");
            log_str = "QNetworkReply::HostNotFoundError";
            break;

        case QNetworkReply::TimeoutError:
            display_str = QObject::tr("Connection timed out");
            log_str = "QNetworkReply::TimeoutError";
            break;

        case QNetworkReply::OperationCanceledError:
            display_str = QObject::tr("Operation cancel error");
            log_str = "QNetworkReply::OperationCanceledError";
            break;

        case QNetworkReply::SslHandshakeFailedError:
            display_str = QObject::tr("SSL/TLS handshake failed");
            log_str = "QNetworkReply::SslHandshakeFailedError";
            break;

        case QNetworkReply::TemporaryNetworkFailureError:
            display_str = QObject::tr("Connection broken");
            log_str = "QNetworkReply::TemporaryNetworkFailureError";
            break;

        case QNetworkReply::NetworkSessionFailedError:
            display_str = QObject::tr("Session failed");
            log_str = "QNetworkReply::NetworkSessionFailedError";
            break;

        case QNetworkReply::BackgroundRequestNotAllowedError:
            display_str = QObject::tr("Background request not allowed");
            log_str = "QNetworkReply::BackgroundRequestNotAllowedError";
            break;

        case QNetworkReply::TooManyRedirectsError:
            display_str = QObject::tr("Too many redirects");
            log_str = "QNetworkReply::TooManyRedirectsError";
            break;

        case QNetworkReply::InsecureRedirectError:
            display_str = QObject::tr("Insecure redirect");
            log_str = "QNetworkReply::InsecureRedirectError";
            break;

        case QNetworkReply::UnknownNetworkError:
            display_str = QObject::tr("Unknown network error");
            log_str = "QNetworkReply::UnknownNetworkError";
            break;

        case QNetworkReply::ProxyConnectionRefusedError:
            display_str = QObject::tr("Connection to proxy refused");
            log_str = "QNetworkReply::ProxyConnectionRefusedError";
            break;

        case QNetworkReply::ProxyConnectionClosedError:
            display_str = QObject::tr("Connection to proxy closed");
            log_str = "QNetworkReply::ProxyConnectionClosedError";
            break;

        case QNetworkReply::ProxyNotFoundError:
            display_str = QObject::tr("Proxy not found");
            log_str = "QNetworkReply::ProxyNotFoundError";
            break;

        case QNetworkReply::ProxyTimeoutError:
            display_str = QObject::tr("Connection to proxy timed out");
            log_str = "QNetworkReply::ProxyTimeoutError";
            break;

        case QNetworkReply::ProxyAuthenticationRequiredError:
            display_str = QObject::tr("Proxy requires authentication");
            log_str = "QNetworkReply::ProxyAuthenticationRequiredError";
            break;

        case QNetworkReply::UnknownProxyError:
            display_str = QObject::tr("Unknown proxy error");
            log_str = "QNetworkReply::UnknownProxyError";
            break;

        case QNetworkReply::ContentAccessDenied:
            display_str = QObject::tr("Access to content denied");
            log_str = "QNetworkReply::ContentAccessDenied";
            break;

        case QNetworkReply::ContentOperationNotPermittedError:
            display_str = QObject::tr("Content operation not permitted");
            log_str = "QNetworkReply::ContentOperationNotPermittedError";
            break;

        case QNetworkReply::ContentNotFoundError:
            display_str = QObject::tr("Content not found");
            log_str = "QNetworkReply::ContentNotFoundError";
            break;

        case QNetworkReply::AuthenticationRequiredError:
            display_str = QObject::tr("Authentication required");
            log_str = "QNetworkReply::AuthenticationRequiredError";
            break;

        case QNetworkReply::ContentReSendError:
            display_str = QObject::tr("Request resend failed");
            log_str = "QNetworkReply::ContentReSendError";
            break;

        case QNetworkReply::ContentConflictError:
            display_str = QObject::tr("Conflict error");
            log_str = "QNetworkReply::ContentConflictError";
            break;

        case QNetworkReply::ContentGoneError:
            display_str = QObject::tr("Content no longer available");
            log_str = "QNetworkReply::ContentGoneError";
            break;

        case QNetworkReply::UnknownContentError:
            display_str = QObject::tr("Unknown content error");
            log_str = "QNetworkReply::UnknownContentError";
            break;

        case QNetworkReply::ProtocolUnknownError:
            display_str = QObject::tr("Protocol unknown error");
            log_str = "QNetworkReply::ProtocolUnknownError";
            break;

        case QNetworkReply::ProtocolInvalidOperationError:
            display_str = QObject::tr("Invalid operation for protocol");
            log_str = "QNetworkReply::ProtocolInvalidOperationError";
            break;

        case QNetworkReply::ProtocolFailure:
            display_str = QObject::tr("Protocol handle error");
            log_str = "QNetworkReply::ProtocolFailure";
            break;

        case QNetworkReply::InternalServerError:
            display_str = QObject::tr("Internal server error");
            log_str = "QNetworkReply::InternalServerError";
            break;

        case QNetworkReply::OperationNotImplementedError:
            display_str = QObject::tr("Server does not support operation");
            log_str = "QNetworkReply::OperationNotImplementedError";
            break;

        case QNetworkReply::ServiceUnavailableError:
            display_str = QObject::tr("Service unavailable");
            log_str = "QNetworkReply::ServiceUnavailableError";
            break;

        case QNetworkReply::UnknownServerError:
            display_str = QObject::tr("Unknown server error");
            log_str = "QNetworkReply::UnknownServerError";
            break;

        default:
            display_str = QObject::tr("Unknown error");
            log_str = QObject::tr("Unknown error");
            break;
    }
    qCritical(qUtf8Printable(QObject::tr("Network error: %s")), qUtf8Printable(log_str));
    errorOccurred(display_str);
}
