/*
  This file is part of KDE Kontact.

  SPDX-FileCopyrightText: 2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#include "kcmkontact.h"
#include "prefs.h"
using namespace Kontact;

#include <KontactInterface/Plugin>

#include <KAboutData>
#include <KLocalizedString>
#include <KServiceTypeTrader>
#include <QComboBox>

#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QStandardItemModel>

extern "C" {
Q_DECL_EXPORT KCModule *create_kontactconfig(QWidget *parent, const char *)
{
    return new KcmKontact(parent);
}
}

KcmKontact::KcmKontact(QWidget *parent)
    : KCModule(parent)
{
    auto topLayout = new QVBoxLayout(this);
    QBoxLayout *pluginStartupLayout = new QHBoxLayout();
    topLayout->addLayout(pluginStartupLayout);

    QCheckBox *forceStartupPluginCheckBox = new QCheckBox(Prefs::self()->forceStartupPluginItem()->label(), this);
    forceStartupPluginCheckBox->setObjectName(QStringLiteral("kcfg_ForceStartupPlugin"));
    pluginStartupLayout->addWidget(forceStartupPluginCheckBox);

    mPluginCombo = new QComboBox(parent);
    mPluginCombo->setToolTip(i18nc("@info:tooltip", "Select the initial plugin to use on each start"));
    mPluginCombo->setWhatsThis(i18nc("@info:whatsthis",
                                     "Select the plugin from this drop down list to be used as the "
                                     "initial plugin each time Kontact is started. Otherwise, Kontact "
                                     "will restore the last active plugin from the previous usage."));
    connect(mPluginCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]() {
        Q_EMIT changed(true);
    });
    pluginStartupLayout->addWidget(mPluginCombo);
    pluginStartupLayout->addStretch();
    mPluginCombo->setEnabled(false);


    connect(forceStartupPluginCheckBox, &QAbstractButton::toggled, mPluginCombo, &QWidget::setEnabled);

    QCheckBox *showSideBarCheckbox = new QCheckBox(Prefs::self()->sideBarOpenItem()->label(), this);
    showSideBarCheckbox->setObjectName(QStringLiteral("kcfg_SideBarOpen"));
    topLayout->addWidget(showSideBarCheckbox);

    addConfig(Prefs::self(), this);
    topLayout->addStretch();
    load();
}

void KcmKontact::load()
{
    const KConfigGroup grp(Prefs::self()->config(), "Plugins");

    const KService::List offers =
        KServiceTypeTrader::self()->query(QStringLiteral("Kontact/Plugin"), QStringLiteral("[X-KDE-KontactPluginVersion] == %1").arg(KONTACT_PLUGIN_VERSION));

    int activeComponent = 0;
    mPluginCombo->clear();
    mPluginList.clear();
    for (const KService::Ptr &service : offers) {
        // skip summary only plugins
        QVariant var = service->property(QStringLiteral("X-KDE-KontactPluginHasPart"));
        if (var.isValid() && var.toBool() == false) {
            continue;
        }
        mPluginCombo->addItem(service->name());
        mPluginList.append(service);

        // skip disabled plugins
        const QString pluginName = service->property(QStringLiteral("X-KDE-PluginInfo-Name")).toString();
        if (!grp.readEntry(pluginName + QStringLiteral("Enabled"), false)) {
            const QStandardItemModel *qsm = qobject_cast<QStandardItemModel *>(mPluginCombo->model());
            if (qsm) {
                qsm->item(mPluginCombo->count() - 1, 0)->setEnabled(false);
            }
        }
        if (service->property(QStringLiteral("X-KDE-PluginInfo-Name")).toString() == Prefs::self()->activePlugin()) {
            activeComponent = mPluginList.count() - 1;
        }
    }

    mPluginCombo->setCurrentIndex(activeComponent);
    KCModule::load();
}

void KcmKontact::save()
{
    const KService::Ptr ptr = mPluginList.at(mPluginCombo->currentIndex());
    const QString activePluginName = ptr->property(QStringLiteral("X-KDE-PluginInfo-Name")).toString();
    Prefs::self()->setActivePlugin(activePluginName);
    KCModule::save();
}

const KAboutData *KcmKontact::aboutData() const
{
    KAboutData *about = new KAboutData(QStringLiteral("kontactconfig"),
                                       i18nc("@title", "KDE Kontact"),
                                       QString(),
                                       QString(),
                                       KAboutLicense::GPL,
                                       i18nc("@info:credit", "(c), 2003 Cornelius Schumacher"));

    about->addAuthor(i18nc("@info:credit", "Cornelius Schumacher"), i18nc("@info:credit", "Developer"), QStringLiteral("schumacher@kde.org"));
    about->addAuthor(i18nc("@info:credit", "Tobias Koenig"), i18nc("@info:credit", "Developer"), QStringLiteral("tokoe@kde.org"));

    return about;
}
