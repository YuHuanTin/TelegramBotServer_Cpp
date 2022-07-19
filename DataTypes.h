//
// Created by 23945 on 2022/7/18.
//

#ifndef TGAPI_DATATYPES_H
#define TGAPI_DATATYPES_H
struct stTg_User{
    long id;
    bool is_bot;
    std::string first_name;
    std::string last_name;
    std::string username;
    std::string language_code;
    bool is_premium;
    bool added_to_attachment_menu;
    bool can_join_groups;
    bool can_read_all_group_messages;
    bool supports_inline_queries;
};
struct stTg_Chat{
    long id;
    std::string type;
    std::string title;
    std::string username;
    std::string first_name;
    std::string last_name;
    ;
    std::string bio;
    bool has_private_forwards;
    bool join_to_send_messages;
    bool join_by_request;
    std::string description;
    std::string invite_link;
    ;
    ;
    long slow_mode_delay;
    long message_auto_delete_time;
    bool has_protected_content;
    std::string sticker_set_name;
    bool can_set_sticker_set;
    long linked_chat_id;
    ;
};
struct stTg_Message{
    stTg_User from;
    stTg_Chat sender_chat;
    long date;
    stTg_Chat chat;
    stTg_User forward_from;
    stTg_Chat forward_from_chat;
    std::string forward_signature;
    std::string forward_sender_name;
    long forward_date;
    bool is_automatic_forward;
    ;
    stTg_User via_bot;
    long edit_date;
    bool has_protected_content;
    std::string media_group_id;
    std::string author_signature;
    std::string text;
    ;
    ;
    ;//audio
    ;//document
    ;//photo
    ;//sticker
    ;//video
    ;//video_note
    ;//voice
    std::string caption;
    ;//caption_entities
    ;//contact
    ;//dice
    ;//game
    ;//poll
    ;//venue
    ;//location
    ;//new_chat_members
    stTg_User left_chat_member;
    std::string new_chat_title;
    ;//new_chat_photo
    bool delete_chat_photo;
    bool group_chat_created;
    bool supergroup_chat_created;
    bool channel_chat_created;
    ;//message_auto_delete_timer_changed
    long migrate_to_chat_id;
    long migrate_from_chat_id;
    ;//pinned_message
    ;//invoice
    ;//successful_payment
    std::string connected_website;
    ;//passport_data
    ;//proximity_alert_triggered
    ;//video_chat_scheduled
    ;//video_chat_started
    ;//video_chat_ended
    ;//video_chat_participants_invited
    ;//web_app_data
    ;//reply_markup
};


#endif //TGAPI_DATATYPES_H
