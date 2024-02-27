#include "FileItemModel.h"

#include "../FileSystem/FileSystemModel.h"
#include "../FileSystem/FileSystemObject.h"
#include "../Util.h"
#include "FileItemModel/Role.h"

using namespace Qml;

namespace Qml {
    using Role = FileItemModelRole;
}

const std::unordered_map<QString, QString> FileItemModel::_icon_name_by_extension_map{
    {"apk", "application-apk.png"},
    {"epub", "application-epub+zip.png"},
    {"geojson", "application-geo+json.png"},
    {"ai", "application-illustrator.png"},
    {"ait", "application-illustrator.png"},
    {"json", "application-json.png"},
    {"xlsx", "application-msexcel.png"},
    {"xlsm", "application-msexcel.png"},
    {"xlsb", "application-msexcel.png"},
    {"xltx", "application-msexcel.png"},
    {"xltm", "application-msexcel.png"},
    {"xls", "application-msexcel.png"},
    {"xlam", "application-msexcel.png"},
    {"xla", "application-msexcel.png"},
    {"xlw", "application-msexcel.png"},
    {"xlr", "application-msexcel.png"},
    {"a", "application-octet-stream.png"},
    {"lib", "application-octet-stream.png"},
    {"obj", "application-octet-stream.png"},
    {"pdf", "application-pdf.png"},
    {"pgp", "application-pgp-encrypted.png"},
    //{"pub", "application-pgp-keys.png"},
    {"asc", "application-pgp-keys.png"},
    {"sig", "application-pgp-signature.png"},
    {"psd", "application-photoshop.png"},
    {"psdc", "application-photoshop.png"},
    {"ps", "application-postscript.png"},
    {"rss", "application-rss+xml.png"},
    {"dart", "application-vnd-dart.png"},
    {"deb", "application-vnd.debian.binary-package.png"},
    {"flatpak", "application-vnd.flatpak.png"},
    {"icc", "application-vnd.iccprofile.png"},
    {"icm", "application-vnd.iccprofile.png"},
    {"adn", "application-vnd.ms-access.png"},
    {"accdb", "application-vnd.ms-access.png"},
    {"accdr", "application-vnd.ms-access.png"},
    {"accdt", "application-vnd.ms-access.png"},
    {"accda", "application-vnd.ms-access.png"},
    {"mdw", "application-vnd.ms-access.png"},
    {"accde", "application-vnd.ms-access.png"},
    {"mam", "application-vnd.ms-access.png"},
    {"maq", "application-vnd.ms-access.png"},
    {"mar", "application-vnd.ms-access.png"},
    {"mat", "application-vnd.ms-access.png"},
    {"maf", "application-vnd.ms-access.png"},
    {"laccdb", "application-vnd.ms-access.png"},
    {"ade", "application-vnd.ms-access.png"},
    {"adp", "application-vnd.ms-access.png"},
    {"mdb", "application-vnd.ms-access.png"},
    {"cdb", "application-vnd.ms-access.png"},
    {"mda", "application-vnd.ms-access.png"},
    {"mdn", "application-vnd.ms-access.png"},
    {"mdf", "application-vnd.ms-access.png"},
    {"mde", "application-vnd.ms-access.png"},
    {"ldb", "application-vnd.ms-access.png"},
    //{"", "application-vnd.ms-excel.png"},
    {"pptx", "application-vnd.ms-powerpoint.png"},
    {"pptm", "application-vnd.ms-powerpoint.png"},
    {"ppt", "application-vnd.ms-powerpoint.png"},
    {"potx", "application-vnd.ms-powerpoint.png"},
    {"potm", "application-vnd.ms-powerpoint.png"},
    {"pot", "application-vnd.ms-powerpoint.png"},
    {"ppsx", "application-vnd.ms-powerpoint.png"},
    {"ppsm", "application-vnd.ms-powerpoint.png"},
    {"pps", "application-vnd.ms-powerpoint.png"},
    {"ppam", "application-vnd.ms-powerpoint.png"},
    {"ppa", "application-vnd.ms-powerpoint.png"},
    {"pub", "application-vnd.ms-publisher.png"},
    {"doc", "application-vnd.ms-word.png"},
    {"dot", "application-vnd.ms-word.png"},
    {"wbk", "application-vnd.ms-word.png"},
    {"docx", "application-vnd.ms-word.png"},
    {"docm", "application-vnd.ms-word.png"},
    {"dotx", "application-vnd.ms-word.png"},
    {"dotm", "application-vnd.ms-word.png"},
    {"docb", "application-vnd.ms-word.png"},
    {"wll", "application-vnd.ms-word.png"},
    {"wwl", "application-vnd.ms-word.png"},
    {"odg", "application-vnd.oasis.opendocument.drawing.png"},
    {"fodg", "application-vnd.oasis.opendocument.drawing.png"},
    {"oxt", "application-vnd.openofficeorg.extension.png"},
    {"snap", "application-vnd.snap.png"},
    {"sfs", "application-vnd.squashfs.png"},
    {"7z", "application-x-7z-compressed.png"},
    //{"", "application-x-addon.png"},
    {"dmg", "application-x-apple-diskimage.png"},
    {"aup3", "application-x-audacity-project.png"},
    {"torrent", "application-x-bittorrent.png"},
    {"iso", "application-x-cd-image.png"},
    {"desktop", "application-x-desktop.png"},
    {"exe", "application-x-executable.png"},
    {"dll", "application-x-executable.png"},
    {"so", "application-x-executable.png"},
    {"bin", "application-x-firmware.png"},
    {"hex", "application-x-firmware.png"},
    {"gzip", "application-x-gzip.png"},
    {"gz", "application-x-gzip.png"},
    {"hwp", "application-x-hwp.png"},
    {"ipynb", "application-x-ipynb+json.png"},
    {"java", "application-x-java.png"},
    {"karbon", "application-x-karbon.png"},
    {"kra", "application-x-krita.png"},
    {"mmpz", "application-x-lmms-project.png"},
    {"mmp", "application-x-lmms-project.png"},
    {"ebook", "application-x-mobipocket-ebook.png"},
    {"model", "application-x-model.png"},
    {"com", "application-x-ms-dos-executable.png"},
    {"url", "application-x-mswinurl.png"},
    {"mscz", "application-x-musescore3.png"},
    {"mscx", "application-x-musescore3.png"},
    {"pl", "application-x-perl.png"},
    {"php", "application-x-php.png"},
    {"p7b", "application-x-pkcs7-certificates.png"},
    {"p7s", "application-x-pkcs7-certificates.png"},
    {"p7m", "application-x-pkcs7-certificates.png"},
    {"p7c", "application-x-pkcs7-certificates.png"},
    {"p7r", "application-x-pkcs7-certificates.png"},
    {"pyc", "application-x-python-bytecode.png"},
    {"pyo", "application-x-python-bytecode.png"},
    {"pyd", "application-x-python-bytecode.png"},
    {"qcow", "application-x-qemu-disk.png"},
    {"rar", "application-x-rar.png"},
    {"img", "application-x-raw-disk-image.png"},
    //{"rss", "application-x-rss+xml.png"},
    //{"sh", "application-x-shellscript.png"},
    {"bat", "application-x-shellscript.png"},
    {"cmd", "application-x-shellscript.png"},
    {"btm", "application-x-shellscript.png"},
    {"srt", "application-x-subrip.png"},
    {"theme", "application-x-theme.png"},
    //{"", "application-x-trash.png"},
    {"hdd", "application-x-virtualbox-hdd.png"},
    {"ova", "application-x-virtualbox-ova.png"},
    {"ovf", "application-x-virtualbox-ovf.png"},
    {"vbox-extpack", "application-x-virtualbox-vbox-extpack.png"},
    {"vbox", "application-x-virtualbox-vbox.png"},
    {"vdi", "application-x-virtualbox-vdi.png"},
    {"vhd", "application-x-virtualbox-vhd.png"},
    {"vmdk", "application-x-virtualbox-vmdk.png"},
    {"yaml", "application-x-yaml.png"},
    {"yml", "application-x-yaml.png"},
    {"zip", "application-x-zip.png"},
    {"midi", "audio-midi.png"},
    {"flac", "audio-x-flac.png"},
    {"ac3", "audio-x-generic.png"},
    {"aa3", "audio-x-generic.png"},
    {"at3", "audio-x-generic.png"},
    {"at9", "audio-x-generic.png"},
    {"atp", "audio-x-generic.png"},
    {"hma", "audio-x-generic.png"},
    {"oma", "audio-x-generic.png"},
    {"omg", "audio-x-generic.png"},
    {"3gp", "audio-x-generic.png"},
    {"aac", "audio-x-generic.png"},
    {"mpc", "audio-x-generic.png"},
    {"mp+", "audio-x-generic.png"},
    {"mpp", "audio-x-generic.png"},
    {"mp3", "audio-x-mpeg.png"},
    {"m3u", "audio-x-mpegurl.png"},
    {"m3u8", "audio-x-mpegurl.png"},
    {"wma", "audio-x-ms-wma.png"},
    {"ogg", "audio-x-vorbis+ogg.png"},
    {"ogv", "audio-x-vorbis+ogg.png"},
    {"oga", "audio-x-vorbis+ogg.png"},
    {"ogx", "audio-x-vorbis+ogg.png"},
    {"ogm", "audio-x-vorbis+ogg.png"},
    {"spx", "audio-x-vorbis+ogg.png"},
    {"opus", "audio-x-vorbis+ogg.png"},
    {"wav", "audio-x-wav.png"},
    {"ttf", "font-x-generic.png"},
    {"otf", "font-x-generic.png"},
    {"woff", "font-x-generic.png"},
    {"woff2", "font-x-generic.png"},
    {"eot", "font-x-generic.png"},
    {"tmpl", "html-template.png"},
    {"svg", "image-svg+xml.png"},
    {"cur", "image-x-cursor.png"},
    {"jpg", "image-x-generic.png"},
    {"jpeg", "image-x-generic.png"},
    {"jpe", "image-x-generic.png"},
    {"jif", "image-x-generic.png"},
    {"jfif", "image-x-generic.png"},
    {"jfi", "image-x-generic.png"},
    {"gif", "image-x-generic.png"},
    {"png", "image-x-generic.png"},
    {"jp2", "image-x-generic.png"},
    {"j2k", "image-x-generic.png"},
    {"jpf", "image-x-generic.png"},
    {"jpm", "image-x-generic.png"},
    {"jpg2", "image-x-generic.png"},
    {"j2c", "image-x-generic.png"},
    {"jpc", "image-x-generic.png"},
    {"jpx", "image-x-generic.png"},
    {"mj2", "image-x-generic.png"},
    {"webp", "image-x-generic.png"},
    {"hdr", "image-x-generic.png"},
    {"heif", "image-x-generic.png"},
    {"heifs", "image-x-generic.png"},
    {"heic", "image-x-generic.png"},
    {"heics", "image-x-generic.png"},
    {"avci", "image-x-generic.png"},
    {"avcs", "image-x-generic.png"},
    {"hif", "image-x-generic.png"},
    {"avif", "image-x-generic.png"},
    {"jxl", "image-x-generic.png"},
    {"tiff", "image-x-generic.png"},
    {"tif", "image-x-generic.png"},
    {"bmp", "image-x-generic.png"},
    {"dib", "image-x-generic.png"},
    {"pbm", "image-x-generic.png"},
    {"pgm", "image-x-generic.png"},
    {"ppm", "image-x-generic.png"},
    {"pnm", "image-x-generic.png"},
    {"bpg", "image-x-generic.png"},
    {"drw", "image-x-generic.png"},
    {"ecw", "image-x-generic.png"},
    {"fits", "image-x-generic.png"},
    {"fit", "image-x-generic.png"},
    {"fts", "image-x-generic.png"},
    {"flif", "image-x-generic.png"},
    {"ico", "image-x-generic.png"},
    {"iff", "image-x-generic.png"},
    {"lbm", "image-x-generic.png"},
    {"jxr", "image-x-generic.png"},
    {"hdp", "image-x-generic.png"},
    {"wdp", "image-x-generic.png"},
    {"liff", "image-x-generic.png"},
    {"nrrd", "image-x-generic.png"},
    {"pam", "image-x-generic.png"},
    {"pgf", "image-x-generic.png"},
    {"sid", "image-x-generic.png"},
    {"xcf", "image-x-xcf.png"},
    //{"", "inode-symlink.png"},
    {"odb", "libreoffice-oasis-database.png"},
    {"fodb", "libreoffice-oasis-database.png"},
    //{"odg", "libreoffice-oasis-drawing.png"},
    //{"fodg", "libreoffice-oasis-drawing.png"},
    {"odf", "libreoffice-oasis-formula.png"},
    {"otm", "libreoffice-oasis-master-document.png"},
    {"odp", "libreoffice-oasis-presentation.png"},
    {"fodp", "libreoffice-oasis-presentation.png"},
    {"ods", "libreoffice-oasis-spreadsheet.png"},
    {"fods", "libreoffice-oasis-spreadsheet.png"},
    {"odt", "libreoffice-oasis-text.png"},
    {"fodt", "libreoffice-oasis-text.png"},
    //{"", "libreoffice-oasis-web.png"},
    {"eml", "message-rfc822.png"},
    {"pkg", "package-x-generic.png"},
    {"rpm", "package-x-generic.png"},
    {"xz", "package-x-generic.png"},
    {"rom", "rom.png"},
    {"sh", "shellscript.png"},
    {"css", "text-css.png"},
    {"docker", "text-dockerfile.png"},
    {"html", "text-html.png"},
    {"less", "text-less.png"},
    {"md", "text-markdown.png"},
    {"markdown", "text-markdown.png"},
    {"rtf", "text-richtext.png"},
    {"rs", "text-rust.png"},
    {"ino", "text-x-arduino.png"},
    //{"", "text-x-authors.png"},
    {"hpp", "text-x-c++hdr.png"},
    {"h", "text-x-chdr.png"},
    {"cbl", "text-x-cobol.png"},
    {"cob", "text-x-cobol.png"},
    {"cpy", "text-x-cobol.png"},
    {"copy", "text-x-copying.png"},
    {"c", "text-x-c.png"},
    {"cpp", "text-x-cpp.png"},
    {"cs", "text-x-csharp.png"},
    {"f90", "text-x-fortran.png"},
    {"f95", "text-x-fortran.png"},
    {"f03", "text-x-fortran.png"},
    {"f", "text-x-fortran.png"},
    {"gcode", "text-x.gcode.png"},
    {"mpt", "text-x.gcode.png"},
    {"mpf", "text-x.gcode.png"},
    {"nc", "text-x.gcode.png"},
    {"txt", "text-x-generic.png"},
    {"po", "text-x-gettext-translation.png"},
    //{"pot", "text-x-gettext-translation-template.png"},
    {"go", "text-x-go.png"},
    {"hs", "text-x-haskell.png"},
    {"lhs", "text-x-haskell.png"},
    {"install", "text-x-install.png"},
    //{"java", "text-x-java.png"},
    {"js", "text-x-javascript.png"},
    {"ly", "text-x-lilypond.png"},
    {"ily", "text-x-lilypond.png"},
    {"log", "text-x-log.png"},
    {"lua", "text-x-lua.png"},
    {"makefile", "text-x-makefile.png"},
    //{"", "text-x-maven+xml.png"},
    //{"", "text-x-meson.png"},
    {"xml", "text-xml.png"},
    {"nim", "text-x-nim.png"},
    {"nims", "text-x-nim.png"},
    {"nimble", "text-x-nim.png"},
    {"patch", "text-x-patch.png"},
    //{"php", "text-x-php.png"},
    //{"", "text-x-preview.png"},
    {"py", "text-x-python.png"},
    {"qml", "text-x-qml.png"},
    {"readme", "text-x-readme.png"},
    {"r", "text-x-r.png"},
    {"rdata", "text-x-r.png"},
    {"rhistory", "text-x-r.png"},
    {"rds", "text-x-r.png"},
    {"rda", "text-x-r.png"},
    {"rb", "text-x-ruby.png"},
    {"ru", "text-x-ruby.png"},
    {"sass", "text-x-sass.png"},
    {"scss", "text-x-sass.png"},
    {"scala", "text-x-scala.png"},
    {"sc", "text-x-scala.png"},
    {"la", "text-x-script.png"},
    {"sql", "text-x-sql.png"},
    {"service", "text-x-systemd-unit.png"},
    {"socket", "text-x-systemd-unit.png"},
    {"device", "text-x-systemd-unit.png"},
    {"mount", "text-x-systemd-unit.png"},
    {"automount", "text-x-systemd-unit.png"},
    {"swap", "text-x-systemd-unit.png"},
    {"target", "text-x-systemd-unit.png"},
    {"path", "text-x-systemd-unit.png"},
    {"timer", "text-x-systemd-unit.png"},
    {"slice", "text-x-systemd-unit.png"},
    {"scope", "text-x-systemd-unit.png"},
    {"tex", "text-x-tex.png"},
    {"ts", "text-x-typescript.png"},
    {"tsx", "text-x-typescript.png"},
    {"vala", "text-x-vala.png"},
    {"vapi", "text-x-vala.png"},
    {"webm", "video-x-generic.png"},
    {"mkv", "video-x-generic.png"},
    {"mk3d", "video-x-generic.png"},
    {"mka", "video-x-generic.png"},
    {"mks", "video-x-generic.png"},
    {"flv", "video-x-generic.png"},
    {"fla", "video-x-generic.png"},
    {"f4v", "video-x-generic.png"},
    {"f4a", "video-x-generic.png"},
    {"f4b", "video-x-generic.png"},
    {"f4p", "video-x-generic.png"},
    {"vob", "video-x-generic.png"},
    {"ifo", "video-x-generic.png"},
    {"bup", "video-x-generic.png"},
    {"drc", "video-x-generic.png"},
    {"gifv", "video-x-generic.png"},
    {"mng", "video-x-generic.png"},
    {"avi", "video-x-generic.png"},
    {"m2t", "video-x-generic.png"},
    {"m2ts", "video-x-generic.png"},
    {"mts", "video-x-generic.png"},
    {"mov", "video-x-generic.png"},
    {"movie", "video-x-generic.png"},
    {"qt", "video-x-generic.png"},
    {"wmv", "video-x-generic.png"},
    {"yuv", "video-x-generic.png"},
    {"rm", "video-x-generic.png"},
    {"rma", "video-x-generic.png"},
    {"rmi", "video-x-generic.png"},
    {"rmv", "video-x-generic.png"},
    {"rmvb", "video-x-generic.png"},
    {"rmhd", "video-x-generic.png"},
    {"rmm", "video-x-generic.png"},
    {"ra", "video-x-generic.png"},
    {"ram", "video-x-generic.png"},
    {"viv", "video-x-generic.png"},
    {"asf", "video-x-generic.png"},
    {"amv", "video-x-generic.png"},
    {"mtv", "video-x-generic.png"},
    {"mp4", "video-x-generic.png"},
    {"m4a", "video-x-generic.png"},
    {"m4p", "video-x-generic.png"},
    {"m4b", "video-x-generic.png"},
    {"m4r", "video-x-generic.png"},
    {"m4v", "video-x-generic.png"},
    {"mpeg", "video-x-generic.png"},
    {"mpg", "video-x-generic.png"},
    {"mpe", "video-x-generic.png"},
    {"mp1", "video-x-generic.png"},
    {"mp2", "video-x-generic.png"},
    {"m1v", "video-x-generic.png"},
    {"m1a", "video-x-generic.png"},
    {"m2a", "video-x-generic.png"},
    {"m2v", "video-x-generic.png"},
    {"mpa", "video-x-generic.png"},
    {"mpv", "video-x-generic.png"},
    {"svi", "video-x-generic.png"},
    {"mxf", "video-x-generic.png"},
    //{"odt", "x-office-document.png"},
    //{"fodt", "x-office-document.png"},
    //{"odp", "x-office-presentation.png"},
    //{"fodp", "x-office-presentation.png"},
    //{"ods", "x-office-spreadsheet.png"},
    //{"fods", "x-office-spreadsheet.png"}
};

const std::unordered_set<QString>  FileItemModel::_special_icon_name_set{
    "application-x-executable.png",
    "application-x-karbon.png",
    "application-x-krita.png",
    "application-x-model.png",
    "application-x-ms-dos-executable.png",
    "folder.png",
    "image-svg+xml.png",
    "image-x-generic.png",
    "image-x-xcf.png",
    "video-x-generic.png"
};

class SizeDisplayer {
public:
    constexpr static size_t size = 7;

    static QString to_string(uint64_t bytes) {
        double sz = bytes;
        int i = 0;
        for (; sz >= 1024 && i < SizeDisplayer::size; ++i)
            sz /= 1024;

        assert(i < _prefixes.size());
        return QString("%1 %2").arg(_locale.toString(sz, 'g', 3), _prefixes[i]);
    }

#ifndef NDEBUG
    static void test() {
        QString str = SizeDisplayer::to_string(1023);
        assert(str.indexOf(QObject::tr("B")) != -1);

        str = SizeDisplayer::to_string(1024);
        assert(str.indexOf(QObject::tr("K")) != -1);

        str = SizeDisplayer::to_string(std::pow(1024, 2) - 1);
        assert(str.indexOf(QObject::tr("K")) != -1);

        str = SizeDisplayer::to_string(std::pow(1024, 2));
        assert(str.indexOf(QObject::tr("M")) != -1);

        str = SizeDisplayer::to_string(std::pow(1024, 3) - 1);
        assert(str.indexOf(QObject::tr("M")) != -1);

        str = SizeDisplayer::to_string(std::pow(1024, 3));
        assert(str.indexOf(QObject::tr("G")) != -1);

        str = SizeDisplayer::to_string(std::pow(1024, 4) - 1);
        assert(str.indexOf(QObject::tr("G")) != -1);

        str = SizeDisplayer::to_string(std::pow(1024, 4));
        assert(str.indexOf(QObject::tr("T")) != -1);

        str = SizeDisplayer::to_string(std::pow(1024, 5) - 1);
        assert(str.indexOf(QObject::tr("T")) != -1);

        str = SizeDisplayer::to_string(std::pow(1024, 5));
        assert(str.indexOf(QObject::tr("P")) != -1);

        //str = SizeDisplayer::to_string(std::pow(1024, 6) - 1); // note: the precision of double type isn't enough; long double isn't supported by Qt
        //assert(str.indexOf(QObject::tr("P")) != -1);

        str = SizeDisplayer::to_string(std::pow(1024, 6));
        assert(str.indexOf(QObject::tr("E")) != -1);

        str = SizeDisplayer::to_string(0xFFFFFFFFFFFFFFFF);
        assert(str.indexOf(QObject::tr("E")) != -1);
    }
#endif

private:
    static const std::array<QString, size> _prefixes;
    static const QLocale _locale;
};

const std::array<QString, SizeDisplayer::size> SizeDisplayer::_prefixes{QObject::tr("B"), QObject::tr("K"), QObject::tr("M"), QObject::tr("G"), QObject::tr("T"), QObject::tr("P"), QObject::tr("E")};
const QLocale SizeDisplayer::_locale;

namespace {
    bool is_valid_dot_pos(const QStringView& name, qsizetype pos) { return pos != -1 && pos != name.size() - 1; }

    QString extract_extension(const QStringView& name, qsizetype dot_pos) { return QStringView(std::begin(name) + dot_pos + 1, std::end(name)).toString().toLower(); }

    QString to_string(std::chrono::sys_seconds t) {
        const time_t c_time = std::chrono::system_clock::to_time_t(t);
        const std::tm tm = *std::localtime(&c_time); // todo: replace with std::chrono::zoned_time() and std::chrono::current_zone(), when GCC will support this
        std::ostringstream stream;
        stream.imbue(std::locale("")); // todo: take into account the translation setting, when it will be introduced
        stream << std::put_time(&tm, "%c");
        return QString::fromStdString(stream.rdbuf()->str());
    }
}

FileItemModel::FileItemModel(std::shared_ptr<::FileSystemModel> model, QObject* parent) : QAbstractListModel(parent), _fs_model(std::move(model)) {
    qDebug().noquote() << QObject::tr("The source file item model is being created");
    _fs_model->add_notification_func(this, std::bind(&FileItemModel::update, this));
    _root = _fs_model->is_cur_dir_root_path();
#ifndef NDEBUG
    std::for_each(std::begin(_icon_name_by_extension_map), std::end(_icon_name_by_extension_map), [](const auto& pair) { const QPixmap pixmap(":/res/icons/" + pair.second); assert(!pixmap.isNull()); });
    SizeDisplayer::test();
#endif
}

FileItemModel::~FileItemModel() {
    qDebug().noquote() << QObject::tr("The source file item model is being destroyed");
    _fs_model->remove_notification_func(this);
}

int FileItemModel::rowCount(const QModelIndex& parent) const { return parent.isValid() ? 0 : _fs_model->size() + (_root ? 0 : 1); }

QVariant FileItemModel::data(const QModelIndex& index, int role) const {
    if (role < to_int(Role::Name) || role >= to_int(Role::EnumSize))
        return QVariant();

    const int row = index.row();
    if (role == to_int(Role::IsExit))
        return !_root && row == 0;

    const FileSystemObject obj = get_object(_root, row);
    switch (to_type<Role>(role)) {
        case Role::Name: {
            return !_root && row == 0 ? ".." : obj.get_name();
        }

        case Role::Extension: {
            if (obj.get_type() == FileSystemObject::Type::Directory)
                return QVariant();

            const QString name = obj.get_name();
            const qsizetype i = name.lastIndexOf('.');
            if (!is_valid_dot_pos(name, i))
                return QVariant();

            return extract_extension(name, i);
        }

        case Role::IconName: {
            return get_icon_name(obj, row);
        }

        case Role::WideImageWidthFlag: {
            const auto it = _special_icon_name_set.find(get_icon_name(obj, row));
            return it != std::end(_special_icon_name_set);
        }

        case Role::CreationTime: {
            if (!obj.is_creation_time_valid())
                return QVariant();

            return QVariant::fromValue(obj.get_creation_time());
        }

        case Role::CreationTimeStr: {
            if (!obj.is_creation_time_valid())
                return QObject::tr("unknown");

            return to_string(obj.get_creation_time());
        }

        case Role::ModTime: {
            if (!obj.is_modification_time_valid())
                return QVariant();

            return QVariant::fromValue(obj.get_modification_time());
        }

        case Role::ModTimeStr: {
            if (!obj.is_modification_time_valid())
                return QObject::tr("unknown");

            return to_string(obj.get_modification_time());
        }

        case Role::FileFlag: {
            return obj.get_type() == FileSystemObject::Type::File;
        }

        case Role::Size: {
            return obj.is_size_valid() ? QVariant::fromValue(obj.get_size()) : QVariant();
        }

        case Role::SizeStr: {
            if (!obj.is_size_valid())
                return QString();

            return SizeDisplayer::to_string(obj.get_size());
        }

        default:
            break;
    }
    return QVariant();
}

QHash<int, QByteArray> FileItemModel::roleNames() const {
    auto names = QAbstractListModel::roleNames();
    names.emplace(to_int(Role::Name), "name");
    names.emplace(to_int(Role::Extension), "extension");
    names.emplace(to_int(Role::IconName), "iconName");
    names.emplace(to_int(Role::WideImageWidthFlag), "needsWideImageWidth");
    names.emplace(to_int(Role::CreationTimeStr), "creationTime");
    names.emplace(to_int(Role::ModTimeStr), "modificationTime");
    names.emplace(to_int(Role::FileFlag), "isFile");
    names.emplace(to_int(Role::SizeStr), "size");
    return names;
}

FileSystemObject FileItemModel::get_object(bool root_path, int row) const {
    if (row == 0)
        return root_path ? _fs_model->get_object(row) : _fs_model->get_curr_dir_object();

    return _fs_model->get_object(row - (root_path ? 0 : 1));
}

QString FileItemModel::get_icon_name(const FileSystemObject& obj, int row) const {
    if (obj.get_type() == FileSystemObject::Type::Directory || !_root && row == 0)
        return QStringLiteral("folder.png");

    const QString name = obj.get_name();
    const qsizetype i = name.lastIndexOf('.');
    if (!is_valid_dot_pos(name, i))
        return QStringLiteral("unknown.png");

    const QString ext = extract_extension(name, i);
    const auto icon_name_it = _icon_name_by_extension_map.find(ext);
    return icon_name_it == std::end(_icon_name_by_extension_map) ? QStringLiteral("unknown.png") : icon_name_it->second;
}

void FileItemModel::update() {
    beginResetModel();
    _root = _fs_model->is_cur_dir_root_path();
    endResetModel();
}
