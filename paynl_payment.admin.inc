<?php

/**
 * @todo Write file documentation.
 */

/**
 * Pay.nl payment methods overview.
 */
function paynl_payment_methods_overview() {
  // Get payment methods based on the PaynlPaymentMethodController.
  $payment_methods = entity_load('payment_method', FALSE,
    array('controller_class_name' => 'PaynlPaymentMethodController'));

  // Prepare a table with all payment methods found.
  $header = array(t('PID'), t('Name'), t('Machine name'), t('Operations'));
  $rows = array();
  foreach ($payment_methods as $payment_method) {
    $row = array(
      $payment_method->pmid,
      $payment_method->title_specific,
      $payment_method->name,
      l(t('settings'), 'admin/config/services/payment/paynl/method/' . $payment_method->pmid),
    );

    $rows[] = $row;
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Pay.nl payment method settings.
 */
function paynl_payment_method_settings_form($form, &$form_state, $pmid) {
  // Load existing settings.
  $settings = _paynl_payment_load_method_settings($pmid);

  // Propagate the pmid of the payment method controller instance.
  $form['pmid'] = array(
    '#type' => 'value',
    '#value' => $pmid,
  );

  // Get configuration data from controller.
  $payment_methods = entity_load('payment_method', FALSE,
    array('controller_class_name' => 'PaynlPaymentMethodController'));
  $payment_method = $payment_methods[$pmid];
  $controller_data = $payment_method->controller_data;

  // Set page title.
  drupal_set_title(t('@title settings', array('@title' => $payment_method->title_specific)));

  // Configure API.
  $api_key = $controller_data['paynl_id'];
  $service_id = $controller_data['paynl_service_id'];
  \Paynl\Config::setApiToken($api_key);
  \Paynl\Config::setServiceId($service_id);

  // Get the Pay.nl payment methods.
  $methods = \Paynl\Paymentmethods::getList();

  // Set default weights if applicable.
  if (empty($settings)) {
    $weight = 0;
    foreach ($methods as $method) {
      $settings[$method['id']] = new stdClass();
      $settings[$method['id']] = $weight;
      $weight++;
    }
  }

  // Add payment methods to the form.
  $form['payment_methods'] = array('#tree' => TRUE);
  $weight_i = -10;
  foreach ($methods as $id => $method) {
    $weight = isset($settings[$id]->weight) ? $settings[$id]->weight : $weight_i++;
    $form['payment_methods'][$weight] = array(
      'enabled' => array(
        '#type' => 'checkbox',
        '#title' => t('Enabled'),
        '#return_value' => 1,
        '#default_value' => isset($settings[$id]->settings['enabled']) ? $settings[$id]->settings['enabled'] : 0,
      ),
      'id' => array(
        '#type' => 'value',
        '#title' => t('ID'),
        '#value' => $method['id'],
      ),
      'name' => array(
        '#type' => 'value',
        '#title' => t('Name'),
        '#value' => $method['name'],
      ),
      'fee' => array(
        '#type' => 'textfield',
        '#title' => t('Transaction fee'),
        '#default_value' => isset($settings[$id]->settings['fee']) ? $settings[$id]->settings['fee'] : '0',
        '#size' => 8,
      ),
      'fee_type' => array(
        '#type' => 'select',
        '#title' => t('Transaction fee type'),
        '#options' => array('amount' => 'amount', 'percentage' => 'percentage'),
        '#default_value' => isset($settings[$id]->settings['fee_type']) ? $settings[$id]->settings['fee_type'] : 'amount',
      ),
      'countries' => array(
        '#type' => 'select',
        '#multiple' => TRUE,
        '#title' => t('Available for'),
        '#options' => country_get_list(),
        '#default_value' => isset($settings[$id]->settings['countries']) ? $settings[$id]->settings['countries'] : array(),
      ),
      'min' => array(
        '#type' => 'textfield',
        '#title' => t('Minimal order amount'),
        '#default_value' => isset($settings[$id]->settings['min']) ? $settings[$id]->settings['min'] : '0',
        '#size' => 8,
      ),
      'max' => array(
        '#type' => 'textfield',
        '#title' => t('Maximum order amount'),
        '#description' => t('Use -1 if there is no maximum amount for this method'),
        '#default_value' => isset($settings[$id]->settings['max']) ? $settings[$id]->settings['max'] : '-1',
        '#size' => 8,
      ),
      'weight' => array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#default_value' => $weight,
        '#delta' => 10,
        '#title_display' => 'invisible',
      ),
    );
  }
  ksort($form['payment_methods']);

  $form['actions'] = array(
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Submit'),
    ),
  );

  $form['#theme'] = 'paynl_payment_method_settings_form';

  return $form;
}

/**
 * Pay.nl payment method settings form validation handler.
 */
function paynl_payment_method_settings_form_validate($form, &$form_state) {
  // We can do any validation on the submitted data here.
}

/**
 * Pay.nl payment method settings form submit handler.
 */
function paynl_payment_method_settings_form_submit($form, &$form_state) {
  $values = $form_state['values'];

  $pmid = $values['pmid'];
  foreach ($values['payment_methods'] as $settings) {
    $id = $settings['id'];
    unset($settings['id']);
    $weight = $settings['weight'];
    unset($settings['weight']);
    db_merge('paynl_payment_method_settings')
      ->fields(array(
        'settings' => serialize($settings),
        'weight' => $weight,
      ))
      ->key(array(
        'id' => $id,
        'pmid' => $pmid,
      ))
      ->execute();
  }
}
