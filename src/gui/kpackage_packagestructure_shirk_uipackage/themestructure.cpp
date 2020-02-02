#include <KLocalizedString>

#include <KPackage/Package>
#include <KPackage/PackageStructure>

#include "utils/stringliterals.h"

namespace Shirk {

using namespace StringLiterals;

class ThemeStructure : public KPackage::PackageStructure
{
    Q_OBJECT
public:
    explicit ThemeStructure(QObject *parent = nullptr, const QVariantList &args = {})
        : KPackage::PackageStructure(parent, args) {}

    void initPackage(KPackage::Package *package) override {
        package->setDefaultPackageRoot(u"shirk/uipackages/"_qs);
        package->addFileDefinition("window", u"main.qml"_qs, i18n("Shirk main window content"));
        package->setRequired("main", true);
    }
};

} // namespace

K_EXPORT_KPACKAGE_PACKAGE_WITH_JSON(Shirk::ThemeStructure, "shirk-packagestructure-uipackage.json")

#include "themestructure.moc"
